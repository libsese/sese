#include "sese/net/V2Server.h"
#include "sese/util/Util.h"
#include "openssl/ssl.h"
#include "sese/Config.h"

#include <sys/_types/_timespec.h>
#include <sys/fcntl.h>
#include <unistd.h>

using namespace sese::net::v2;

inline static int setNonblocking(socket_t socket) {
    auto option = fcntl(socket, F_GETFL);
    if (-1 != option) {
        return fcntl(socket, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

DarwinServiceIOContext::DarwinServiceIOContext(socket_t socket, void *ssl) noexcept
    : socket(socket),
      ssl(ssl) {
}

int64_t DarwinServiceIOContext::peek(void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_peek((ssl_st *) ssl, (char *) buf, (int) len);
    } else {
        return ::recv(socket, (char *) buf, (int) len, MSG_PEEK);
    }
}

int64_t DarwinServiceIOContext::read(void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_read((ssl_st *) ssl, (char *) buf, (int) len);
    } else {
        return ::recv(socket, (char *) buf, (int) len, 0);
    }
}

int64_t DarwinServiceIOContext::write(const void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_write((ssl_st *) ssl, (const char *) buf, (int) len);
    } else {
        return ::send(socket, (const char *) buf, (int) len, 0);
    }
}

void DarwinServiceIOContext::close() noexcept {
    if (ssl) {
        ::SSL_shutdown((ssl_st *) ssl);
    } else {
        ::shutdown(socket, SHUT_RDWR);
    }
    isClosing = true;
}

DarwinService::Ptr DarwinService::create(ServerOption *opt) noexcept {
    if (opt == nullptr) {
        return nullptr;
    }

    if (opt->isSSL && opt->sslContext) {
        if (!opt->sslContext->authPrivateKey()) {
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

    auto server = new DarwinService;
    server->option = opt;
    server->socket = sock;
    server->kqueue = kqueue;

    return std::unique_ptr<DarwinService>(server);
}

void DarwinService::loop() noexcept {
    int numberOfFds;
    struct timespec timeout {
        1, 0
    };
    while (true) {
        if (exit) break;

        numberOfFds = ::kevent(kqueue, nullptr, 0, eventSet, MaxEventSize, &timeout);
        if (-1 == numberOfFds) continue;
        for (int i = 0; i < numberOfFds; ++i) {
            if (eventSet[i].ident == socket) {
                socket_t clientSocket = ::accept(socket, nullptr, nullptr);

                if (setNonblocking(clientSocket)) {
                    ::close(clientSocket);
                    continue;
                }

                if (option->isSSL) {
                    auto *clientSSL = (ssl_st *) handshake(clientSocket);
                    if (clientSSL == nullptr) {
                        ::close(clientSocket);
                        continue;
                    }

                    struct kevent event {};
                    EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, clientSSL);
                    ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
                } else {
                    struct kevent event {};
                    EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
                    ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
                }
            } else if (eventSet[i].filter == EVFILT_READ) {
                if (eventSet[i].flags & EV_EOF) {
                    if (option->isSSL) {
                        auto *clientSSL = (ssl_st *) eventSet[i].udata;
                        SSL_free(clientSSL);
                    }
                    close((int) eventSet[i].ident);
                    continue;
                } else {
                    handle({(socket_t) eventSet[i].ident, eventSet[i].udata});
                    continue;
                }
            }
        }
    }
}

void *DarwinService::handshake(socket_t client) noexcept {
    ssl_st *clientSSL = nullptr;
    clientSSL = SSL_new((SSL_CTX *) option->sslContext->getContext());
    SSL_set_fd(clientSSL, (int) client);
    SSL_set_accept_state(clientSSL);

    while (true) {
        auto rt = SSL_do_handshake(clientSSL);
        if (rt <= 0) {
            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
            auto err = SSL_get_error(clientSSL, rt);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                SSL_free(clientSSL);
                close(client);
                return nullptr;
            }
        } else {
            break;
        }
    }

    return clientSSL;
}

void DarwinService::handle(DarwinServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;

        if (option->beforeHandle(myCtx)) {
            option->onHandle(myCtx);
        }

        if (!myCtx.isClosing) {
            struct kevent event {};
            EV_SET(&event, myCtx.socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, myCtx.ssl);
            ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
        }
    });
}


void DarwinService::start() noexcept {
    mainThread = std::make_unique<Thread>([this]() { loop(); }, "LINUX_MAIN");
    threadPool = std::make_unique<ThreadPool>("LINUX_SERV", option->threads);
    mainThread->start();
}

void DarwinService::shutdown() noexcept {
    if (mainThread != nullptr && mainThread->joinable()) {
        exit = true;
        mainThread->join();
        threadPool->shutdown();
        close(socket);
        close(kqueue);
    }
}