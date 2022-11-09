#include <sese/net/TcpServer.h>
#include <sese/Util.h>
#include <fcntl.h>

using sese::Socket;
using Server = sese::TcpServer;

int64_t sese::IOContext::read(void *buffer, size_t length) {
    return ::read(socket, buffer, length);
}

int64_t sese::IOContext::write(const void *buffer, size_t length) {
    return ::write(socket, buffer, length);
}

void sese::IOContext::close() {
    isClosed = true;
    shutdown(socket, SHUT_RDWR);
    ::close(socket);
}

#define CLEAR      \
    close(sockFd); \
    return nullptr;

Server::Ptr Server::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockFd) {
        return nullptr;
    }

    int32_t opt = fcntl(sockFd, F_GETFL);
    if (-1 == opt) {
        CLEAR
    }

    if (-1 == fcntl(sockFd, F_SETFL, opt | O_NONBLOCK)) {
        CLEAR
    }

    if (-1 == ::bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
        CLEAR
    }

    if (-1 == ::listen(sockFd, SERVER_MAX_CONNECTION)) {
        CLEAR
    }

    auto epollFd = EpollCreate(0);
    if (-1 == epollFd) {
        CLEAR
    }

    EpollEvent event{};
    event.events = EPOLLIN;
    event.data.fd = sockFd;
    if (EpollCtl(epollFd, EPOLL_CTL_ADD, sockFd, &event)) {
        close(epollFd);
        close(sockFd);
        return nullptr;
    }

    auto server = new Server;
    server->sockFd = sockFd;
    server->epollFd = epollFd;
    server->threadPool = std::make_unique<ThreadPool>("TcpServer", threads);
    server->ioContextPool = ObjectPool<IOContext>::create();
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<Server>(server);
}

#undef CLEAR

Server::Ptr Server::create(const Socket::Ptr &listenSocket, size_t threads, size_t keepAlive) noexcept {
    auto epollFd = EpollCreate(0);
    if (-1 == epollFd) {
        return nullptr;
    }

    auto sockFd = listenSocket->getRawSocket();

    EpollEvent event{};
    event.events = EPOLLIN;
    event.data.fd = sockFd;
    if (EpollCtl(epollFd, EPOLL_CTL_ADD, sockFd, &event)) {
        close(epollFd);
        return nullptr;
    }

    auto server = new Server;
    server->sockFd = sockFd;
    server->epollFd = epollFd;
    server->threadPool = std::make_unique<ThreadPool>("TcpServer", threads);
    server->ioContextPool = ObjectPool<IOContext>::create();
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<Server>(server);
}

void Server::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
    int32_t nfds;
    socket_t clientFd;
    EpollEvent event{};
    while (true) {
        if (isShutdown) break;

        nfds = EpollWait(epollFd, events, MaxEvents, 0);
        if (-1 == nfds) continue;
        for (int32_t i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockFd) {
                // 新连接接入
                clientFd = ::accept(sockFd, nullptr, nullptr);
                if (-1 == clientFd) continue;

                int32_t opt = fcntl(sockFd, F_GETFL);
                if (-1 == opt) {
                    close(clientFd);
                    continue;
                }

                if (-1 == fcntl(sockFd, F_SETFL, opt | O_NONBLOCK)) {
                    close(clientFd);
                    continue;
                }

                event.events = EPOLLIN | EPOLLONESHOT;
                event.data.fd = clientFd;
                if (-1 == EpollCtl(epollFd, EPOLL_CTL_ADD, clientFd, &event)) {
                    close(clientFd);
                    continue;
                }

                // 首次接入，开始计时
                if (0 != keepAlive) {
                    mutex.lock();
                    taskMap[clientFd] = timer->delay(std::bind(&Server::closeCallback, this, clientFd), (int64_t) keepAlive, false);
                    mutex.unlock();
                }
            } else if (events[i].events & EPOLLIN) {
                // 缓冲区可读
                if (events[i].events & EPOLLHUP) {
                    // FIN 标识，断开连接，不做处理
                    continue;
                }
                clientFd = events[i].data.fd;
                if (0 != keepAlive) {
                    mutex.lock();
                    auto iterator = taskMap.find(clientFd);
                    // iterator != taskMap.end()
                    taskMap.erase(clientFd);
                    mutex.unlock();
                    iterator->second->cancel();
                }

                auto ioContext = ioContextPool->borrow();
                ioContext->socket = clientFd;
                threadPool->postTask([handler, ioContext, this]() {
                    handler(ioContext.get());

                    if (ioContext->isClosed) {
                        // 不需要保留连接，已主动关闭
                    } else {
                        if (0 == keepAlive) {
                            ::shutdown(ioContext->socket, SHUT_RDWR);
                            ::close(ioContext->socket);
                        } else {
                            // 需要保留连接，但需要做超时管理
                            EpollEvent ev{};
                            ev.events = EPOLLIN | EPOLLONESHOT;
                            ev.data.fd = ioContext->socket;
                            EpollCtl(epollFd, EPOLL_CTL_MOD, ioContext->socket, &ev);

                            // 继续计时
                            mutex.lock();
                            taskMap[ioContext->socket] = timer->delay(std::bind(&Server::closeCallback, this, ioContext->socket), (int64_t) keepAlive, false);
                            mutex.unlock();

                            // 重置标识符
                            ioContext->isClosed = false;
                        }
                    }
                });
            }
        }
    }
}

void Server::shutdown() noexcept {
    isShutdown = true;
    threadPool->shutdown();
    timer->shutdown();
    for (auto &pair: taskMap) {
        ::shutdown(pair.first, SHUT_RDWR);
        close(pair.first);
    }
}

void Server::closeCallback(socket_t socket) noexcept {
    mutex.lock();
    taskMap.erase(socket);
    mutex.unlock();
    ::shutdown(socket, SHUT_RDWR);
    close(socket);
}