//#include "sese/security/SecurityTcpServer.h"
//#include "openssl/ssl.h"
//
//#include <ctime>
//#include <unistd.h>
//#include <fcntl.h>
//
//int64_t sese::security::IOContext::read(void *buf, size_t length) {// NOLINT
//    return SSL_read((SSL *) ssl, buf, (int) length);
//}
//
//int64_t sese::security::IOContext::write(const void *buf, size_t length) {// NOLINT
//    return SSL_write((SSL *) ssl, buf, (int) length);
//}
//
//void sese::security::IOContext::close() {
//    SSL_shutdown((SSL *) ssl);
//    SSL_free((SSL *) ssl);
//    shutdown(socket, SHUT_RDWR);
//    ::close(socket);
//    isClosed = true;
//}
//
//sese::security::SecurityTcpServer::Ptr sese::security::SecurityTcpServer::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive, SSLContext::Ptr &ctx) noexcept {
//    auto family = ipAddress->getRawAddress()->sa_family;
//    socket_t sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
//    if (-1 == sockFd) {
//        return nullptr;
//    }
//
//    int32_t opt = fcntl(sockFd, F_GETFL);
//    if (-1 == opt) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    if (-1 == fcntl(sockFd, F_SETFL, opt | O_NONBLOCK)) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    if (-1 == ::bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    if (-1 == ::listen(sockFd, SERVER_MAX_CONNECTION)) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    if (!ctx->authPrivateKey()) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    int32_t kqueueFd = kqueue();
//    if (-1 == kqueueFd) {
//        ::close(sockFd);
//        return nullptr;
//    }
//
//    KEvent event{};
//    EV_SET(&event, sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
//    if (-1 == kevent(kqueueFd, &event, 1, nullptr, 0, nullptr)) {
//        close(sockFd);
//        close(kqueueFd);
//        return nullptr;
//    }
//
//    auto server = new SecurityTcpServer;
//    server->ctx = ctx;
//    server->sockFd = sockFd;
//    server->kqueueFd = kqueueFd;
//    server->threadPool = std::make_unique<ThreadPool>("SecurityTcpServer", threads);
//    server->ioContextPool = ObjectPool<IOContext>::create();
//    if (keepAlive > 0) {
//        server->timer = Timer::create();
//    }
//    server->keepAlive = keepAlive;
//    return std::unique_ptr<SecurityTcpServer>(server);
//}
//
//void sese::security::SecurityTcpServer::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
//    struct timespec timeout {
//        1, 0
//    };
//
//    while (true) {
//        if (isShutdown) break;
//
//        int32_t nev = kevent(kqueueFd, nullptr, 0, events, MaxEvents, &timeout);
//        if (-1 == nev) continue;
//        for (int32_t n = 0; n < nev; n++) {
//            if (events[n].ident == sockFd) {
//                // 新连接接入
//                socket_t client = accept(sockFd, nullptr, nullptr);
//                if (-1 == client) {
//                    close(client);
//                    continue;
//                }
//
//                int32_t opt = fcntl(client, F_GETFL);
//                if (-1 == opt) {
//                    close(client);
//                    continue;
//                }
//
//                if (-1 == fcntl(client, F_SETFL, opt | O_NONBLOCK)) {
//                    close(client);
//                    continue;
//                }
//
//                auto clientSSL = SSL_new((SSL_CTX *) ctx->getContext());
//                SSL_set_fd(clientSSL, (int) client);
//                SSL_set_accept_state(clientSSL);
//                while (true) {
//                    auto rt = SSL_do_handshake(clientSSL);
//                    if (rt <= 0) {
//                        // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
//                        auto err = SSL_get_error(clientSSL, rt);
//                        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
//                            SSL_free(clientSSL);
//                            close(client);
//                            clientSSL = nullptr;
//                            break;
//                        }
//                    } else {
//                        break;
//                    }
//                }
//                if (clientSSL == nullptr) {
//                    continue;
//                }
//
//                KEvent event{};
//                EV_SET(&event, client, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
//                if (-1 == kevent(kqueueFd, &event, 1, nullptr, 0, nullptr)) {
//                    close(client);
//                    continue;
//                }
//
//                auto clientCtx = ioContextPool->borrow();
//                clientCtx->socket = client;
//                clientCtx->ssl = clientSSL;
//
//                mutex.lock();
//                contextMap[client] = clientCtx;
//                // 首次接入，开始计时
//                if (0 != keepAlive) {
//                    clientCtx->task = timer->delay(std::bind(&SecurityTcpServer::closeCallback, this, client), (int64_t) keepAlive, false);
//                }
//                mutex.unlock();
//            } else if (events[n].filter == EVFILT_READ) {
//                // 缓冲区可读
//                if (events[n].flags & EV_EOF) {
//                    // FIN 标识
//                    mutex.lock();
//                    auto iterator = contextMap.find((socket_t) events[n].ident);
//                    // iterator != contextMap.end();
//                    contextMap.erase(iterator);
//                    if (iterator->second->task != nullptr) {
//                        iterator->second->task->cancel();
//                        iterator->second->task = nullptr;
//                    }
//                    mutex.unlock();
//                    SSL_free((SSL *) iterator->second->ssl);
//                    ::shutdown(iterator->second->socket, SHUT_RDWR);
//                    ::close(iterator->second->socket);
//                    continue;
//                }
//
//                auto client = (socket_t) events[n].ident;
//                mutex.lock();
//                auto iterator = contextMap.find(client);
//                if (0 != keepAlive) {
//                    // 取消定时任务
//                    // iterator != taskMap.end()
//                    iterator->second->task->cancel();
//                    iterator->second->task = nullptr;
//                }
//                mutex.unlock();
//
//                // 提交任务
//                threadPool->postTask([handler, ioContext = iterator->second, this]() {
//                    handler(ioContext.get());
//
//                    if (ioContext->isClosed) {
//                        // 不需要保留连接，已主动关闭
//                        mutex.lock();
//                        auto iterator = contextMap.find(ioContext->socket);
//                        if (iterator != contextMap.end()) {
//                            if (iterator->second->task != nullptr) {
//                                iterator->second->task->cancel();
//                                iterator->second->task = nullptr;
//                            }
//                            contextMap.erase(iterator);
//                        }
//                        mutex.unlock();
//                    } else {
//                        if (0 == keepAlive) {
//                            // 此处不应默认关闭链接，应由开发人员决定
//                            // SSL_shutdown((SSL *) ioContext->ssl);
//                            // SSL_free((SSL *) ioContext->ssl);
//                            // ::shutdown(ioContext->socket, SHUT_RDWR);
//                            // ::close(ioContext->socket);
//                            mutex.lock();
//                            auto iterator = contextMap.find(ioContext->socket);
//                            contextMap.erase(iterator);
//                            mutex.unlock();
//                        } else {
//                            // 需要保留连接，但需要做超时管理
//                            KEvent ev{};
//                            EV_SET(&ev, ioContext->socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
//                            kevent(kqueueFd, &ev, 1, nullptr, 0, nullptr);
//
//                            mutex.lock();
//                            // 继续计时
//                            ioContext->task = timer->delay(std::bind(&SecurityTcpServer::closeCallback, this, ioContext->socket), (int64_t) keepAlive, false);
//                            // 重置标识符
//                            ioContext->isClosed = false;
//                            mutex.unlock();
//                        }
//                    }
//                });
//            }
//        }
//    }
//}
//
//void sese::security::SecurityTcpServer::shutdown() noexcept {
//    isShutdown = true;
//    threadPool->shutdown();
//    if (keepAlive > 0) {
//        timer->shutdown();
//    }
//    SSL *toFree;
//    for (auto &pair: contextMap) {
//        toFree = (SSL *) pair.second->ssl;
//        SSL_shutdown(toFree);
//        SSL_free(toFree);
//        ::shutdown(pair.first, SHUT_RDWR);
//        close(pair.first);
//    }
//    close(kqueueFd);
//}
//
//void sese::security::SecurityTcpServer::closeCallback(socket_t socket) noexcept {
//    SSL *toFree;
//    mutex.lock();
//    auto iterator = contextMap.find(socket);
//    toFree = (SSL *) iterator->second->ssl;
//    contextMap.erase(iterator);
//    mutex.unlock();
//    SSL_shutdown(toFree);
//    SSL_free(toFree);
//    ::shutdown(socket, SHUT_RDWR);
//    close(socket);
//}