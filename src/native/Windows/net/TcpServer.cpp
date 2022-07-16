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
    socket_t sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockFd) {
        return nullptr;
    }

#define CLEAR            \
    closesocket(sockFd); \
    return nullptr;

    unsigned long ul = 1;
    if (-1 == ioctlsocket(sockFd, FIONBIO, &ul)) {
        CLEAR
    }

    if (-1 == bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
        CLEAR
    }

    if (-1 == listen(sockFd, SERVER_MAX_CONNECTION)) {
        CLEAR
    }

    WSAEVENT listenEvent = WSACreateEvent();
    if (-1 == WSAEventSelect(sockFd, listenEvent, FD_ACCEPT | FD_CLOSE)) {
        WSACloseEvent(listenEvent);
        CLEAR
    }

    auto server = new Server;
    server->socks[0] = sockFd;
    server->events[0] = listenEvent;
    server->eventNum++;
    server->threadPool = std::make_unique<ThreadPool>("TcpServer", threads);
    server->ioContextPool = ObjectPool<IOContext>::create();
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<Server>(server);
}

void Server::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
    while (true) {
        if (isShutdown) break;

        DWORD index = WSAWaitForMultipleEvents(eventNum, events, FALSE, WSA_INFINITE, FALSE);
        if (index == WSA_WAIT_FAILED) continue;

        index -= WSA_WAIT_EVENT_0;
        for (DWORD i = index; i < eventNum; i++) {
            WSANETWORKEVENTS networkEvents;
            if (SOCKET_ERROR == WSAEnumNetworkEvents(socks[i], events[i], &networkEvents)) continue;

            if (networkEvents.lNetworkEvents & FD_ACCEPT) {
                if (0 != networkEvents.iErrorCode[FD_ACCEPT_BIT]) continue;
                // 新连接接入
                SOCKET client = accept(socks[0], nullptr, nullptr);
                if (INVALID_SOCKET == client) continue;

                // nonblocking
                unsigned long ul = 1;
                if (SOCKET_ERROR == ioctlsocket(client, FIONBIO, &ul)) {
                    closesocket(client);
                    continue;
                }

                // 注册 WSA EVENT
                WSAEVENT event = WSACreateEvent();
                if (SOCKET_ERROR == WSAEventSelect(client, event, FD_READ | FD_CLOSE)) {
                    WSACloseEvent(event);
                    closesocket(client);
                    continue;
                }

                socks[eventNum] = client;
                events[eventNum] = event;
                eventNum++;
                sockStatus[client] = true;

                if (0 != keepAlive) {
                    mutex.lock();
                    taskMap[client] = timer->delay([this, client]() { this->closeCallback(client); }, (int64_t) keepAlive, false);
                    mutex.unlock();
                }
            } else if (networkEvents.lNetworkEvents & FD_READ) {
                if (0 != networkEvents.iErrorCode[FD_READ_BIT]) continue;
                if (!sockStatus[socks[i]]) continue;

                auto client = socks[i];

                if (0 != keepAlive) {
                    mutex.lock();
                    auto iterator = taskMap.find(client);
                    // iterator != taskMap.end()
                    auto task = iterator->second;
                    taskMap.erase(client);
                    mutex.unlock();
                    task->cancel();
                }

                auto ioContext = ioContextPool->borrow();
                ioContext->socket = socks[i];
                threadPool->postTask([handler, ioContext, this]() {
                    handler(ioContext.get());

                    auto client = ioContext->socket;

                    if (ioContext->isClosed) {
                        // 不需要保留连接，已主动关闭
                    } else {
                        if (0 == keepAlive) {
                            ::shutdown(ioContext->socket, SD_BOTH);
                            ::closesocket(ioContext->socket);
                        } else {
                            // 需要保留连接，但需要做超时管理
                            // 重置状态
                            sockStatus[client] = true;

                            // 继续计时
                            mutex.lock();
                            taskMap[client] = timer->delay([this, client]() { this->closeCallback(client); }, (int64_t) keepAlive, false);
                            mutex.unlock();

                            // 重置标识符
                            ioContext->isClosed = false;
                        }
                    }
                });

            } else if (networkEvents.lNetworkEvents & FD_CLOSE) {
                sockStatus.erase(socks[i]);
                ::closesocket(socks[i]);
                for (DWORD j = i; j < eventNum - 1; j++) {
                    socks[j] = socks[j + 1];
                    events[j] = events[j + 1];
                }
                eventNum--;
            }
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