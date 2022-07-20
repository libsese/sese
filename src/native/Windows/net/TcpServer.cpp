#include <sese/net/TcpServer.h>
#include <sese/Util.h>

#pragma warning(disable : 4267)

using sese::IOContext;
using Server = sese::TcpServer;

int64_t sese::IOContext::read(void *buffer, size_t length) {
    return ::recv(socket, (char *) buffer, length, 0);
}

int64_t sese::IOContext::write(const void *buffer, size_t length) {
    return ::send(socket, (const char *) buffer, length, 0);
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
    server->threadArray.emplace_back(std::make_unique<Thread>([server]() { server->forwardFunction(); }, "IOCP0"));
    server->threadArray.emplace_back(std::make_unique<Thread>([server]() { server->forwardFunction(); }, "IOCP1"));

    server->listenSock = sockFd;
    server->hIOCP = hIOCP;
    server->threadPool = std::make_unique<ThreadPool>("TcpServer", threads);
    server->ioContextPool = ObjectPool<IOContext>::create();
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<Server>(server);
}

void Server::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
    for(Thread::Ptr &th : threadArray) {
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
//        postRecv(client);

        DWORD dwFlags = 0;
        DWORD nBytes = MaxBufferSize;
        auto *ioContext = new IOContext;
        int32_t nRet = WSARecv(client, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
        if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
            perror("not io pending!!!");
            delete ioContext;
            continue;
        }
    }
}

void Server::shutdown() noexcept {
    isShutdown = true;
    threadPool->shutdown();
    timer->shutdown();
    for (auto &pair: taskMap) {
        ::shutdown(pair.first, SD_BOTH);
        closesocket(pair.first);
    }
}

void Server::closeCallback(socket_t socket) noexcept {
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
    if (SOCKET_ERROR == nRt) {
        // 读取发生错误
        if (ERROR_IO_PENDING != WSAGetLastError()) {
            // ERROR_IO_PENDING 以外的错误统统关闭处理
            closesocket(ioContext->socket);
        delete ioContext;
        }
    }
}

void Server::forwardFunction() noexcept {
    IOContext *ioContext = nullptr;
    DWORD nBytes = MaxBufferSize;
    DWORD dwFlags = 0;
    DWORD dwIoContextSize;
    void *lpCompletionKey = nullptr;
    while (true) {
        GetQueuedCompletionStatus(hIOCP, &dwIoContextSize, (PULONG_PTR) &lpCompletionKey, (LPOVERLAPPED *) &ioContext, INFINITE);
        if (dwIoContextSize == 0) {
            closesocket(ioContext->socket);
            delete ioContext;
            ioContext = nullptr;
            continue;
        }

        // 只处理首次读事件
        if (ioContext->operation == Operation::Read) {
            int nRt = WSARecv(ioContext->socket, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &(ioContext->overlapped), nullptr);
            if (SOCKET_ERROR == nRt) {
                // 读取发生错误
                if (ERROR_IO_PENDING != WSAGetLastError()) {
                    // ERROR_IO_PENDING 以外的错误统统关闭处理
                    closesocket(ioContext->socket);
                }
                delete ioContext;
                ioContext = nullptr;
                continue;
            } else {
                // 已经读取到数据 - 触发首次事件
                // ...
                puts(ioContext->buffer);
                delete ioContext;
                ioContext = nullptr;
            }
        }
    }
}