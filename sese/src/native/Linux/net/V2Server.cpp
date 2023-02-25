#include "sese/net/V2Server.h"
#include "openssl/ssl.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int64_t sese::net::v2::IOContext::read(void *buffer, size_t length) noexcept {// NOLINT
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) length);
    } else {
        return ::read(socket, buffer, (int) length);
    }
}

int64_t sese::net::v2::IOContext::write(const void *buffer, size_t length) noexcept {// NOLINT
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) length);
    } else {
        return ::write(socket, buffer, length);
    }
}

void sese::net::v2::IOContext::close() noexcept {
    if (ssl) {
        isClosed = true;
        // SSL_shutdown((SSL *) ssl);
        SSL_free((SSL *) ssl);
        ::close(socket);
    } else {
        isClosed = true;
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
    }
}

inline static int setNonblocking(socket_t socket) {
    auto option = fcntl(socket, F_GETFL);
    if (option != -1) {
        return fcntl(socket, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

sese::net::v2::Server::Ptr sese::net::v2::Server::create(const sese::net::v2::ServerOption &opt) noexcept {
    if (opt.isSSL) {
        if (opt.sslContext) {
            if (!opt.sslContext->authPrivateKey()) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    socket_t sock = ::socket(opt.address->getRawAddress()->sa_family, SOCK_STREAM, IPPROTO_IP);
    if (-1 == sock) {
        return nullptr;
    }

    if (-1 == setNonblocking(sock)) {
        ::close(sock);
        return nullptr;
    }

    if (-1 == bind(sock, opt.address->getRawAddress(), opt.address->getRawAddressLength())) {
        ::close(sock);
        return nullptr;
    }

    if (-1 == listen(sock, SERVER_MAX_CONNECTION)) {
        ::close(sock);
        return nullptr;
    }

    int epoll = epoll_create1(0);
    if (-1 == epoll) {
        ::close(sock);
        return nullptr;
    }

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = sock;
    epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &event);

    auto server = new Server;
    server->option = opt;
    server->socket = sock;
    server->epoll = epoll;

    if (opt.isKeepAlive && opt.keepAlive > 0) {
        server->timer = Timer::create();
    }

    server->threads = std::make_unique<ThreadPool>(
            "SERV",
            opt.threads
    );

    return std::unique_ptr<Server>(server);
}

void sese::net::v2::Server::LinuxWorkerFunction(sese::net::v2::IOContext *ctx) noexcept {
    bool isHandle = option.beforeHandle(ctx);
    if (isHandle) {
        option.onHandle(ctx);
    }

    // 启用了长连接
    if (option.isKeepAlive && option.keepAlive > 0) {
        // 连接已经关闭，清理资源
        if (ctx->isClosed) {
            mutex.lock();
            contextMap.erase(ctx->socket);
            mutex.unlock();
            delete ctx;
        }
        // 连接未关闭，重新提交管理并做超时管理
        else {
            epoll_event event{};
            event.data.fd = ctx->socket;
            event.events = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT;
            epoll_ctl(epoll, EPOLL_CTL_MOD, ctx->socket, &event);

            ctx->task = timer->delay(
                    [this, client = ctx->socket]() {
                        mutex.lock();
                        auto iterator = contextMap.find(client);
                        if (iterator != contextMap.end()) {
                            iterator->second->task = nullptr;
                            contextMap.erase(iterator);
                            mutex.unlock();
                            printf("连接关闭 %d\n", client);
                            iterator->second->close();
                            delete iterator->second;
                        } else {
                            // Never reach
                            mutex.unlock();
                        }
                    },
                    option.keepAlive, false
            );
        }
    }
    // 开启了自动关闭
    else if (option.autoClose) {
        // 连接已经关闭
        if (ctx->isClosed) {
            printf("连接关闭 %d\n", ctx->socket);
            mutex.lock();
            contextMap.erase(ctx->socket);
            mutex.unlock();
            delete ctx;
        } else {
            printf("连接关闭 %d\n", ctx->socket);
            ctx->close();
            mutex.lock();
            contextMap.erase(ctx->socket);
            mutex.unlock();
            delete ctx;
        }
    }
}

void sese::net::v2::Server::loop() noexcept {
    int numberOfFds;
    epoll_event event{};
    while (true) {
        if (isShutdown) break;

        numberOfFds = epoll_wait(epoll, events, MaxEventSize, 0);
        if (-1 == numberOfFds) continue;
        for (int i = 0; i < numberOfFds; ++i) {
            if (events[i].data.fd == socket) {
                socket_t client = ::accept(socket, nullptr, nullptr);
                if (-1 == client) continue;

                if (-1 == setNonblocking(client)) {
                    ::close(client);
                    continue;
                }

                SSL *clientSSL = nullptr;
                if (option.isSSL) {
                    clientSSL = SSL_new((SSL_CTX *) option.sslContext->getContext());
                    SSL_set_fd(clientSSL, (int) client);
                    SSL_set_accept_state(clientSSL);
                    printf("尝试握手\n");
                    while (true) {
                        auto rt = SSL_do_handshake(clientSSL);
                        if (rt <= 0) {
                            sleep(0);
                            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                            auto err = SSL_get_error(clientSSL, rt);
                            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                                printf("握手失败\n");
                                SSL_free(clientSSL);
                                ::close(client);
                                clientSSL = nullptr;
                                break;
                            }
                            printf("重试\n");
                        } else {
                            printf("握手成功\n");
                            break;
                        }
                    }
                    if (clientSSL == nullptr) {
                        continue;
                    }
                }

                event.events = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT;
                event.data.fd = client;
                if (-1 == epoll_ctl(epoll, EPOLL_CTL_ADD, client, &event)) {
                    if (option.isSSL) {
                        SSL_shutdown(clientSSL);
                        SSL_free(clientSSL);
                        ::close(client);
                    } else {
                        ::shutdown(client, SHUT_RDWR);
                        ::close(client);
                    }
                    continue;
                }

                printf("新连接 %d\n", client);
                auto ctx = new sese::net::v2::IOContext;
                ctx->socket = client;
                ctx->ssl = clientSSL;

                mutex.lock();
                contextMap[client] = ctx;
                mutex.unlock();
                if (option.isKeepAlive && option.keepAlive > 0) {
                    ctx->task = timer->delay(
                            [this, client]() {
                                mutex.lock();
                                auto iterator = contextMap.find(client);
                                if (iterator != contextMap.end()) {
                                    iterator->second->task = nullptr;
                                    printf("连接关闭 %d\n", client);
                                    iterator->second->close();
                                    contextMap.erase(iterator);
                                    mutex.unlock();
                                    delete iterator->second;
                                } else {
                                    // Never reach
                                    mutex.unlock();
                                }
                            },
                            option.keepAlive, false
                    );
                }
            } else if (events[i].events & EPOLLIN) {
                // 缓冲区可读
                socket_t client = events[i].data.fd;
                if (events[i].events & EPOLLRDHUP) {
                    // FIN 标识
                    mutex.lock();
                    auto iterator = contextMap.find(client);
                    if (iterator != contextMap.end()) {
                        contextMap.erase(iterator);
                        mutex.unlock();
                        if (option.isKeepAlive && option.keepAlive > 0 && iterator->second->task) {
                            iterator->second->task->cancel();
                            iterator->second->task = nullptr;
                        }
                        printf("连接关闭 %d\n", iterator->first);
                        iterator->second->close();
                        delete iterator->second;
                    }
                    mutex.unlock();
                    continue;
                }
                // 正常读取
                else {
                    mutex.lock();
                    auto iterator = contextMap.find(client);
                    if (option.isKeepAlive && option.keepAlive > 0 && iterator->second->task) {
                        iterator->second->task->cancel();
                        iterator->second->task = nullptr;
                    }
                    mutex.unlock();
                    auto *ctx = iterator->second;
                    threads->postTask([this, ctx] { LinuxWorkerFunction(ctx); });
                }
            }
        }
    }
}

void sese::net::v2::Server::shutdown() noexcept {
    isShutdown = true;
    threads->shutdown();
    timer->shutdown();
    for (auto &pair: contextMap) {
        if (option.isKeepAlive && option.keepAlive > 0) {
            pair.second->task->cancel();
            pair.second->task = nullptr;
        }
        printf("连接关闭 %d\n", pair.first);
        pair.second->close();
        delete pair.second;
    }
    ::close(epoll);
    ::close(socket);
}