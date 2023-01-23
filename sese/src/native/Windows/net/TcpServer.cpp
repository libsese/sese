#include <sese/net/TcpServer.h>
#include "sese/util/Util.h"

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
    ::shutdown(socket, SD_BOTH);
    ::closesocket(socket);
}

#define CLEAR            \
    closesocket(sockFd); \
    return nullptr;

Server::Ptr Server::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t sockFd = ::WSASocketW(family, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (-1 == sockFd) {
        return nullptr;
    }

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

    auto server = new Server;

    server->threads = threads;
    for (size_t index = 0; index < threads; index++) {
        server->threadGroup.emplace_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "IOCP" + std::to_string(index)));
    }

    server->listenSock = sockFd;
    server->hIOCP = hIOCP;

    if (keepAlive > 0) {
        server->timer = Timer::create();
        server->keepAlive = keepAlive;
    }

    return std::unique_ptr<Server>(server);
}

#undef CLEAR

Server::Ptr Server::create(const Socket::Ptr &listenSocket, size_t threads, size_t keepAlive) noexcept {
    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, threads);
    if (INVALID_HANDLE_VALUE == hIOCP) {
        return nullptr;
    }

    auto server = new Server;

    server->threads = threads;
    for (size_t index = 0; index < threads; index++) {
        server->threadGroup.emplace_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "IOCP" + std::to_string(index)));
    }

    server->listenSock = listenSocket->getRawSocket();
    server->hIOCP = hIOCP;

    if (keepAlive > 0) {
        server->timer = Timer::create();
        server->keepAlive = keepAlive;
    }

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
        auto ioContext = new IOContext;
        ioContext->socket = client;
#ifdef _DEBUG
        printf("NEW: %p\n", ioContext);
#endif

        if (0 != keepAlive) {
            mutex.lock();
            taskMap[ioContext] = timer->delay([this, ioContext]() { this->WindowsCloseCallback(ioContext); }, (int64_t) keepAlive, false);
            mutex.unlock();
        }

        DWORD nBytes = MaxBufferSize;
        DWORD dwFlags = 0;
        int nRt = WSARecv(client, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
        auto e = WSAGetLastError();
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
            if (keepAlive > 0) {
                WindowsCloseCallback(ioContext);
            } else {
#ifdef _DEBUG
                printf("CLOSE: %p\n", ioContext);
#endif
                closesocket(client);
                delete ioContext;
            }
        }
    }
}

void Server::shutdown() noexcept {
    void *lpCompletionKey = nullptr;
    isShutdown = true;
    for (auto i = 0; i < threads; i++) {
        PostQueuedCompletionStatus(hIOCP, -1, (ULONG_PTR) lpCompletionKey, nullptr);
    }
    if (keepAlive > 0) {
        timer->shutdown();
    }
    for (auto &pair: taskMap) {
        ::shutdown(pair.first->socket, SD_BOTH);
        closesocket(pair.first->socket);
#ifdef _DEBUG
        printf("CLOSE: %p\n", pair.first);
#endif
        delete pair.first;
    }
    for (auto &thread: threadGroup) {
        thread->join();
    }
}

void Server::WindowsCloseCallback(IOContext *ioContext) noexcept {
#ifdef _DEBUG
    printf("CLOSE: %p\n", ioContext);
#endif
    mutex.lock();
    taskMap.erase(ioContext);
    mutex.unlock();
    ::shutdown(ioContext->socket, SD_BOTH);
    closesocket(ioContext->socket);
    delete ioContext;
}

void Server::WindowsWorkerFunction() noexcept {
    IOContext *ioContext = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    DWORD dwFlags = 0;
    DWORD nBytes = MaxBufferSize;

    while (true) {
        BOOL bRt = GetQueuedCompletionStatus(
                hIOCP,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ioContext,
                INFINITE
        );

        if (!bRt) continue;
        if (lpNumberOfBytesTransferred == -1) break;
        if (lpNumberOfBytesTransferred == 0) continue;
        if (lpNumberOfBytesTransferred == MaxBufferSize) {
#ifdef _DEBUG
            printf("BAD: %p\n", ioContext);
#endif
            closesocket(ioContext->socket);
            delete ioContext;
            ioContext = nullptr;
            continue;
        }
        ioContext->nBytes = lpNumberOfBytesTransferred;

        // 只处理首次读事件
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
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
            // 读取发生错误
#ifdef _DEBUG
            printf("CLOSE: %p\n", ioContext);
#endif
            closesocket(ioContext->socket);
            delete ioContext;
            ioContext = nullptr;
            continue;
        } else {
            // 已经读取到数据 - 触发首次事件
            // ...
#ifdef _DEBUG
            printf("RECV: %p\n", ioContext);
#endif
            auto client = ioContext->socket;
            if (0 != keepAlive) {
                mutex.lock();
                auto iterator = taskMap.find(ioContext);
                if (iterator != taskMap.end()) {
                    auto task = iterator->second;
                    taskMap.erase(ioContext);
                    mutex.unlock();
                    task->cancel();
                } else {
                    mutex.unlock();
                }
            }

            handler(ioContext);

            if (ioContext->isClosed) {
                // 不需要保留连接，已主动关闭
#ifdef _DEBUG
                printf("CLOSE: %p\n", ioContext);
#endif
                delete ioContext;
            } else {
                if (0 == keepAlive) {
#ifdef _DEBUG
                    printf("CLOSE: %p\n", ioContext);
#endif
                    ::shutdown(ioContext->socket, SD_BOTH);
                    ::closesocket(ioContext->socket);
                    delete ioContext;
                } else {
                    // 继续计时
                    mutex.lock();
                    taskMap[ioContext] = timer->delay([this, ioContext]() { this->WindowsCloseCallback(ioContext); }, (int64_t) keepAlive, false);
                    mutex.unlock();

                    // 再次提交读取
                    ioContext->nRead = 0;
                    ioContext->nBytes = 0;
                    nRt = WSARecv(client, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
                    e = WSAGetLastError();
                    if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
                        this->WindowsCloseCallback(ioContext);
                    } else {
#ifdef _DEBUG
                        printf("POST: %p\n", ioContext);
#endif
                    }
                }
            }
        }
    }
}