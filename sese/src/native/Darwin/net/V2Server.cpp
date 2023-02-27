#include "sese/net/V2Server.h"
#include "openssl/ssl.h"
#include "sese/Config.h"

#include <sys/_types/_timespec.h>
#include <sys/fcntl.h>
#include <unistd.h>

int64_t sese::net::v2::IOContext::read(void *buf, size_t length) noexcept {// NOLINT
    if (ssl) {
        return SSL_read((SSL *) ssl, buf, (int) length);
    } else {
        return ::read(socket, buf, length);
    }
}

int64_t sese::net::v2::IOContext::write(const void *buf, size_t length) noexcept {// NOLINT
    if (ssl) {
        return SSL_write((SSL *) ssl, buf, (int) length);
    } else {
        return ::write(socket, buf, length);
    }
}

void sese::net::v2::IOContext::close() noexcept {
    if (ssl) {
        isClosed = true;
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
    if (-1 != option) {
        return fcntl(socket, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

sese::net::v2::Server::Ptr sese::net::v2::Server::create(sese::net::v2::ServerOption *opt) noexcept {
    if (opt->isSSL) {
        if (opt->sslContext) {
            if (!opt->sslContext->authPrivateKey()) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    socket_t sock = ::socket(opt->address->getRawAddress()->sa_family, SOCK_STREAM, IPPROTO_IP);
    if (-1 == sock) {
        return nullptr;
    }

    if (-1 == setNonblocking(sock)) {
        ::close(sock);
        return nullptr;
    }

    if (-1 == ::bind(sock, opt->address->getRawAddress(), opt->address->getRawAddressLength())) {
        ::close(sock);
        return nullptr;
    }

    if (-1 == ::listen(sock, SERVER_MAX_CONNECTION)) {
        ::close(sock);
        return nullptr;
    }

    auto kqueue = ::kqueue();
    if (-1 == kqueue) {
        ::close(sock);
        return nullptr;
    }

    struct kevent event {};
    EV_SET(&event, sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    if (-1 == ::kevent(kqueue, &event, 1, nullptr, 0, nullptr)) {
        ::close(kqueue);
        ::close(sock);
        return nullptr;
    }

    auto server = new Server;
    server->option = opt;
    server->socket = sock;
    server->kqueue = kqueue;

    if (opt->isKeepAlive && opt->keepAlive > 0) {
        server->timer = Timer::create();
    }

    server->threads = std::make_unique<ThreadPool>(
            "SERV",
            opt->threads
    );

    return std::unique_ptr<Server>(server);
}

void sese::net::v2::Server::onConnect() noexcept {
    struct kevent event {};
    socket_t client = ::accept(socket, nullptr, nullptr);
    if (-1 == client) return;

    if (-1 == setNonblocking(client)) {
        ::close(client);
        return;
    }

    SSL *clientSSL = nullptr;
    if (option->isSSL) {
        clientSSL = SSL_new((SSL_CTX *) option->sslContext->getContext());
        SSL_set_fd(clientSSL, (int) client);
        SSL_set_accept_state(clientSSL);
        while (true) {
            auto rt = SSL_do_handshake(clientSSL);
            if (rt <= 0) {
                sleep(0);
                // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                auto err = SSL_get_error(clientSSL, rt);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    SSL_free(clientSSL);
                    close(client);
                    clientSSL = nullptr;
                    break;
                }
            } else {
                break;
            }
        }
        if (clientSSL == nullptr) {
            return;
        }
    }

    EV_SET(&event, client, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
    if (-1 == ::kevent(kqueue, &event, 1, nullptr, 0, nullptr)) {
        if (option->isSSL) {
            SSL_shutdown(clientSSL);
            SSL_free(clientSSL);
            ::close(client);
        } else {
            ::shutdown(client, SHUT_RDWR);
            ::close(client);
        }
        return;
    }

    auto ctx = new sese::net::v2::IOContext;
    ctx->socket = client;
    ctx->ssl = clientSSL;
    mutex.lock();
    contextMap[client] = ctx;
    mutex.unlock();
    if (option->isKeepAlive && option->keepAlive > 0) {
        ctx->task = timer->delay(
                [this, client]() {
                    mutex.lock();
                    auto iterator = contextMap.find(client);
                    if (iterator != contextMap.end()) {
                        iterator->second->task = nullptr;
                        iterator->second->close();
                        contextMap.erase(iterator);
                        mutex.unlock();
                        delete iterator->second;
                    } else {
                        // Never reach
                        mutex.unlock();
                    }
                },
                option->keepAlive, false
        );
    }
}

void sese::net::v2::Server::onClose(socket_t client) noexcept {
    mutex.lock();
    auto iterator = contextMap.find(client);
    if (iterator != contextMap.end()) {
        contextMap.erase(iterator);
        mutex.unlock();
        if (option->isKeepAlive && option->keepAlive > 0 && iterator->second->task) {
            iterator->second->task->cancel();
            iterator->second->task = nullptr;
        }
        iterator->second->close();
        delete iterator->second;
    } else {
        mutex.unlock();
    }
}

void sese::net::v2::Server::onRead(socket_t client) noexcept {
    mutex.lock();
    auto iterator = contextMap.find(client);
    if (option->isKeepAlive && option->keepAlive > 0 && iterator->second->task) {
        iterator->second->task->cancel();
        iterator->second->task = nullptr;
    }
    mutex.unlock();
    threads->postTask([this, ctx = iterator->second] { DarwinWorkerFunction(ctx); });
}

void sese::net::v2::Server::DarwinWorkerFunction(sese::net::v2::IOContext *ctx) noexcept {
    bool isHandle = option->beforeHandle(ctx);
    if (isHandle) {
        option->onHandle(ctx);
    }

    // 启用了长连接
    if (option->isKeepAlive && option->keepAlive > 0) {
        // 连接已经关闭，清理资源
        if (ctx->isClosed) {
            mutex.lock();
            contextMap.erase(ctx->socket);
            mutex.unlock();
            delete ctx;
        }
        // 连接未关闭， 需要重新提交管理并做超时管理
        else {
            struct kevent event {};
            EV_SET(&event, ctx->socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
            ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);

            ctx->task = timer->delay(
                    [this, ctx]() {
                        mutex.lock();
                        auto iterator = contextMap.find(ctx->socket);
                        if (iterator != contextMap.end()) {
                            iterator->second->task = nullptr;
                            contextMap.erase(iterator);
                            mutex.unlock();
                            iterator->second->close();
                            delete iterator->second;
                        } else {
                            // Never reach
                            mutex.unlock();
                        }
                    },
                    option->keepAlive, false
            );
        }
    }
    // 开启了自动关闭
    else if (option->autoClose) {
        // 连接已经关闭
        if (ctx->isClosed) {
            mutex.lock();
            contextMap.erase(ctx->socket);
            mutex.unlock();
            delete ctx;
        } else {
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
    struct timespec timeout {
        1, 0
    };
    while (true) {
        if (isShutdown) break;

        numberOfFds = ::kevent(kqueue, nullptr, 0, events, MaxEventSize, &timeout);
        if (-1 == numberOfFds) continue;
        for (int i = 0; i < numberOfFds; ++i) {
            if (events[i].ident == socket) {
                // 新连接接入
                onConnect();
            } else if (events[i].filter == EVFILT_READ) {
                if (events[i].flags & EV_EOF) {
                    onClose((socket_t) events[i].ident);
                } else {
                    onRead((socket_t) events[i].ident);
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
        if (option->isKeepAlive && option->keepAlive > 0) {
            pair.second->task->cancel();
            pair.second->task = nullptr;
        }
        pair.second->close();
        delete pair.second;
    }
    ::close(kqueue);
    ::close(socket);
}