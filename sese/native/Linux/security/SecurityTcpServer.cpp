#include "sese/security/SecurityTcpServer.h"
#include "openssl/ssl.h"

#include <fcntl.h>

int64_t sese::security::IOContext::read(void *buf, size_t length) {
    return SSL_read((SSL *) ssl, buf, (int) length);
}

int64_t sese::security::IOContext::write(const void *buf, size_t length) {
    return SSL_write((SSL *) ssl, buf, (int) length);
}

void sese::security::IOContext::close() {
    SSL_shutdown((SSL *) ssl);
    SSL_free((SSL *) ssl);
    shutdown(socket, SHUT_RDWR);
    ::close(socket);
    isClosed = true;
}

sese::security::SecurityTcpServer::Ptr sese::security::SecurityTcpServer::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive, SSLContext::Ptr &ctx) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockFd) {
        return nullptr;
    }

    int32_t opt = fcntl(sockFd, F_GETFL);
    if (-1 == opt) {
        close(sockFd);
        return nullptr;
    }

    if (-1 == fcntl(sockFd, F_SETFL, opt | O_NONBLOCK)) {
        close(sockFd);
        return nullptr;
    }

    if (-1 == ::bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
        close(sockFd);
        return nullptr;
    }

    if (-1 == ::listen(sockFd, SERVER_MAX_CONNECTION)) {
        close(sockFd);
        return nullptr;
    }

    auto epollFd = EpollCreate(0);
    if (-1 == epollFd) {
        close(sockFd);
        return nullptr;
    }

    if (!ctx->authPrivateKey()) {
        close(sockFd);
        return nullptr;
    }

    EpollEvent event{};
    event.events = EPOLLIN;
    event.data.fd = sockFd;
    if (EpollCtl(epollFd, EPOLL_CTL_ADD, sockFd, &event)) {
        close(epollFd);
        close(sockFd);
        return nullptr;
    }

    auto server = new SecurityTcpServer;
    server->ctx = ctx;
    server->sockFd = sockFd;
    server->epollFd = epollFd;
    server->threadPool = std::make_unique<ThreadPool>("SecurityTcpServer", threads);
    server->ioContextPool = ObjectPool<IOContext>::create();
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<SecurityTcpServer>(server);
}

void sese::security::SecurityTcpServer::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
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

                auto clientSSL = SSL_new((SSL_CTX *) ctx->getContext());
                SSL_set_fd(clientSSL, (int) clientFd);
                SSL_set_accept_state(clientSSL);
                while (true) {
                    auto rt = SSL_do_handshake(clientSSL);
                    if (rt <= 0) {
                        // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                        auto err = SSL_get_error(clientSSL, rt);
                        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                            SSL_free(clientSSL);
                            close(clientFd);
                            clientSSL = nullptr;
                            break;
                        }
                    } else {
                        break;
                    }
                }
                if (clientSSL == nullptr) {
                    continue;
                }

                event.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
                event.data.fd = clientFd;
                if (-1 == EpollCtl(epollFd, EPOLL_CTL_ADD, clientFd, &event)) {
                    close(clientFd);
                    continue;
                }

                auto clientCtx = ioContextPool->borrow();
                clientCtx->socket = clientFd;
                clientCtx->ssl = clientSSL;

                mutex.lock();
                contextMap[clientFd] = clientCtx;
                // 首次接入，开始计时
                if (0 != keepAlive) {
                    clientCtx->task = timer->delay(std::bind(&SecurityTcpServer::closeCallback, this, clientFd), (int64_t) keepAlive, false);
                }
                mutex.unlock();
            } else if (events[i].events & EPOLLIN) {
                // 缓冲区可读
                if (events[i].events & EPOLLRDHUP) {
                    // FIN 标识，断开连接，不做处理
                    close(events[i].data.fd);
                    continue;
                }
                clientFd = events[i].data.fd;
                Map::iterator iterator;
                if (0 != keepAlive) {
                    mutex.lock();
                    iterator = contextMap.find(clientFd);
                    // iterator != taskMap.end()
                    mutex.unlock();
                    iterator->second->task->cancel();
                }

                threadPool->postTask([handler, iterator, this]() {
                    auto ioContext = iterator->second;
                    handler(ioContext.get());

                    if (ioContext->isClosed) {
                        // 不需要保留连接，已主动关闭
                        mutex.lock();
                        contextMap.erase(iterator);
                        mutex.unlock();
                    } else {
                        if (0 == keepAlive) {
                            SSL_shutdown((SSL *)ioContext->ssl);
                            SSL_free((SSL *)ioContext->ssl);
                            ::shutdown(ioContext->socket, SHUT_RDWR);
                            ::close(ioContext->socket);
                            mutex.lock();
                            contextMap.erase(iterator);
                            mutex.unlock();
                        } else {
                            // 需要保留连接，但需要做超时管理
                            EpollEvent ev{};
                            ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
                            ev.data.fd = ioContext->socket;
                            EpollCtl(epollFd, EPOLL_CTL_MOD, ioContext->socket, &ev);

                            // 继续计时
                            mutex.lock();
                            ioContext->task = timer->delay(std::bind(&SecurityTcpServer::closeCallback, this, ioContext->socket), (int64_t) keepAlive, false);
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

void sese::security::SecurityTcpServer::shutdown() noexcept {
    isShutdown = true;
    threadPool->shutdown();
    timer->shutdown();
    SSL *toFree;
    for (auto &pair: contextMap) {
        toFree = (SSL *) pair.second->ssl;
        SSL_shutdown(toFree);
        SSL_free(toFree);
        ::shutdown(pair.first, SHUT_RDWR);
        close(pair.first);
    }
    close(epollFd);
}

void sese::security::SecurityTcpServer::closeCallback(socket_t socket) noexcept {
    SSL *toFree;
    mutex.lock();
    auto iterator = contextMap.find(socket);
    toFree = (SSL *)iterator->second->ssl;
    contextMap.erase(iterator);
    mutex.unlock();
    SSL_shutdown(toFree);
    SSL_free(toFree);
    ::shutdown(socket, SHUT_RDWR);
    close(socket);
}