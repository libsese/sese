#include <sese/net/TcpServer.h>
#include <sese/Util.h>

#pragma warning(disable : 4267)

using sese::IOContext;
using Server = sese::TcpServer;

int64_t sese::IOContext::read(void *buffer, size_t length) {
    // 缓冲区内有未读字节
    if (this->nRead < this->nBytes) {
        // 缓冲区够用
        if (this->nBytes - this->nRead > length) {
            memcpy(buffer, this->buffer + this->nRead, length);
            this->nRead += length;
            return (int64_t) length;
        }
        // 缓冲区不够用
        // 直接返回当前剩余部分
        else {
            memcpy(buffer, this->buffer + this->nRead, this->nBytes - this->nRead);
            this->nRead = this->nBytes;
            return this->nBytes - this->nRead;
        }
    }
    // 缓冲区已空
    else {
        return ::recv(socket, (char *) buffer, (int32_t) length, 0);
    }
}

int64_t sese::IOContext::write(const void *buffer, size_t length) {
    return ::send(socket, (const char *) buffer, (int32_t) length, 0);
}

void sese::IOContext::close() {
    isClosed = true;
    shutdown(socket, SD_BOTH);
    ::closesocket(socket);
}

Server::Ptr Server::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t sockFd = ::WSASocketW(family, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (-1 == sockFd) {
        return nullptr;
    }

#define CLEAR            \
    closesocket(sockFd); \
    return nullptr;

    unsigned long ul = 1;
    if (SOCKET_ERROR == ioctlsocket(sockFd, FIONBIO, &ul)) {
        CLEAR
    }

    if (SOCKET_ERROR == bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
        CLEAR
    }

    if (SOCKET_ERROR == listen(sockFd, SERVER_MAX_CONNECTION)) {
        CLEAR
    }

    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, threads);
    if (INVALID_HANDLE_VALUE == hIOCP) {
        CLEAR
    }
#undef CLEAR

    auto server = new Server;

    server->threads = threads;
    for (size_t index = 0; index < threads; index++) {
        server->threadGroup.emplace_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "IOCP" + std::to_string(index)));
    }

    server->listenSock = sockFd;
    server->hIOCP = hIOCP;
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<Server>(server);
}

void Server::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
    this->handler = handler;

    for (Thread::Ptr &th: threadGroup) {
        th->start();
    }

    while (true) {
        if (isShutdown) break;

        SOCKET client = ::accept(listenSock, nullptr, nullptr);
        if (SOCKET_ERROR == client) continue;

        unsigned long ul = 1;
        if (SOCKET_ERROR == ioctlsocket(client, FIONBIO, &ul)) {
            closesocket(client);
            continue;
        }

        if (nullptr == CreateIoCompletionPort((HANDLE) client, hIOCP, 0, 0)) {
            closesocket(client);
            continue;
        }

        // 首次接入
        if (0 != keepAlive) {
            mutex.lock();
            taskMap[client] = timer->delay([this, client]() { this->closeCallback(client); }, (int64_t) keepAlive, false);
            mutex.unlock();
        }
        postRecv(client);
    }
}

void Server::shutdown() noexcept {
    void *lpCompletionKey = nullptr;
    isShutdown = true;
    for (auto i = 0; i < threads; i++) {
        PostQueuedCompletionStatus(hIOCP, -1, (ULONG_PTR) lpCompletionKey, nullptr);
    }
    timer->shutdown();
    for (auto &pair: taskMap) {
        ::shutdown(pair.first, SD_BOTH);
        closesocket(pair.first);
    }
    for (auto &thread: threadGroup) {
        thread->join();
    }
}

void Server::closeCallback(socket_t socket) noexcept {
    puts("CLOSE");
    mutex.lock();
    taskMap.erase(socket);
    mutex.unlock();
    ::shutdown(socket, SD_BOTH);
    closesocket(socket);
}

void Server::postRecv(SOCKET socket) noexcept {
    auto ioContext = new IOContext;
    ioContext->socket = socket;
    ioContext->operation = Operation::Read;

    DWORD nBytes = MaxBufferSize;
    DWORD dwFlags = 0;
    int nRt = WSARecv(socket, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
    auto e = WSAGetLastError();
    if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
        closesocket(ioContext->socket);
        delete ioContext;
    }
}

void Server::WindowsWorkerFunction() noexcept {
    IOContext *ioContext = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    DWORD dwFlags = 0;
    DWORD nBytes = MaxBufferSize;

    while (true) {
        GetQueuedCompletionStatus(
                hIOCP,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ioContext,
                INFINITE
        );

        if (lpNumberOfBytesTransferred == -1) break;

        if (lpNumberOfBytesTransferred == 0) {
            ioContext = nullptr;
            continue;
        }

        ioContext->nBytes = lpNumberOfBytesTransferred;

        // 只处理首次读事件
        if (ioContext->operation == Operation::Read) {
            int nRt = WSARecv(
                    ioContext->socket,
                    &ioContext->wsaBuf,
                    1,
                    &nBytes,
                    &dwFlags,
                    &(ioContext->overlapped),
                    nullptr
            );
            auto e = WSAGetLastError();
            if (SOCKET_ERROR == nRt && e != WSAGetLastError()) {
                // 读取发生错误
                closesocket(ioContext->socket);
                delete ioContext;
                ioContext = nullptr;
                continue;
            } else {
                // 已经读取到数据 - 触发首次事件
                // ...
                auto client = ioContext->socket;
                if (0 != keepAlive) {
                    mutex.lock();
                    auto iterator = taskMap.find(client);
                    // iterator != taskMap.end()
                    if (iterator != taskMap.end()) {
                        auto task = iterator->second;
                        taskMap.erase(client);
                        mutex.unlock();
                        task->cancel();
                    }
                }

                handler(ioContext);

                if (ioContext->isClosed) {
                    // 不需要保留连接，已主动关闭
                } else {
                    if (0 == keepAlive) {
                        ::shutdown(ioContext->socket, SD_BOTH);
                        ::closesocket(ioContext->socket);
                    } else {
                        // 继续计时
                        mutex.lock();
                        taskMap[client] = timer->delay([this, client]() { this->closeCallback(client); }, (int64_t) keepAlive, false);
                        mutex.unlock();

                        // failed
                        postRecv(client);
                    }
                }

                delete ioContext;
                ioContext = nullptr;
                continue;
            }
        }
    }
}