#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/V2Server.h"
#include "sese/util/Util.h"
#include "openssl/ssl.h"
#include "sese/Config.h"

#include <sys/_types/_timespec.h>
#include <sys/fcntl.h>
#include <unistd.h>

using namespace sese::net::v2;

inline static int setNonblocking(sese::socket_t socket) {
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

// DarwinService::Ptr DarwinService::create(ServerOption *opt) noexcept {
//     if (opt == nullptr) {
//         return nullptr;
//     }

//     if (opt->isSSL && opt->sslContext) {
//         if (!opt->sslContext->authPrivateKey()) {
//             return nullptr;
//         }
//     }

//     socket_t sock = ::socket(opt->address->getRawAddress()->sa_family, SOCK_STREAM, IPPROTO_IP);
//     if (-1 == sock) {
//         return nullptr;
//     }

//     if (-1 == setNonblocking(sock)) {
//         ::close(sock);
//         return nullptr;
//     }

//     if (-1 == ::bind(sock, opt->address->getRawAddress(), opt->address->getRawAddressLength())) {
//         ::close(sock);
//         return nullptr;
//     }

//     if (-1 == ::listen(sock, SERVER_MAX_CONNECTION)) {
//         ::close(sock);
//         return nullptr;
//     }

//     auto kqueue = ::kqueue();
//     if (-1 == kqueue) {
//         ::close(sock);
//         return nullptr;
//     }

//     struct kevent event {};
//     EV_SET(&event, sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
//     if (-1 == ::kevent(kqueue, &event, 1, nullptr, 0, nullptr)) {
//         ::close(kqueue);
//         ::close(sock);
//         return nullptr;
//     }

//     auto server = new DarwinService;
//     server->option = opt;
//     server->socket = sock;
//     server->kqueue = kqueue;

//     return std::unique_ptr<DarwinService>(server);
// }

DarwinService::~DarwinService() noexcept {
    if (initStatus && !exitStatus && mainThread) {
        shutdown();
    }
}

bool DarwinService::init() noexcept {
    if (sslContext && !sslContext->authPrivateKey()) {
        return false;
    }

    if (!address) {
        address = IPv4Address::localhost();
        address->setPort(8080);
    }

    socket = ::socket(address->getRawAddress()->sa_family, SOCK_STREAM, IPPROTO_IP);
    if (-1 == socket) {
        return false;
    }
    if (-1 == setNonblocking(socket)) {
        ::close(socket);
        return false;
    }
    if (-1 == ::bind(socket, address->getRawAddress(), address->getRawAddressLength())) {
        ::close(socket);
        return false;
    }
    if (-1 == ::listen(socket, SERVER_MAX_CONNECTION)) {
        ::close(socket);
        return false;
    }
    kqueue = ::kqueue();
    if (-1 == kqueue) {
        ::close(socket);
        return false;
    }
    struct kevent event {};
    EV_SET(&event, socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    if (-1 == ::kevent(kqueue, &event, 1, nullptr, 0, nullptr)) {
        ::close(kqueue);
        ::close(socket);
        return false;
    }

    initStatus = true;
    return true;
}

void DarwinService::loop() noexcept {
    int numberOfFds;
    struct timespec timeout {
        1, 0
    };
    while (true) {
        if (exitStatus) break;

        numberOfFds = ::kevent(kqueue, nullptr, 0, eventSet, MaxEventSize, &timeout);
        if (-1 == numberOfFds) continue;
        for (int i = 0; i < numberOfFds; ++i) {
            if (eventSet[i].ident == socket) {
                socket_t clientSocket = ::accept(socket, nullptr, nullptr);

                if (setNonblocking(clientSocket)) {
                    ::close(clientSocket);
                    continue;
                }

                if (sslContext) {
                    auto *clientSSL = (ssl_st *) handshake(clientSocket);
                    if (clientSSL == nullptr) {
                        ::close(clientSocket);
                        continue;
                    }

                    connect({clientSocket, clientSSL});

                    struct kevent event {};
                    EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, clientSSL);
                    ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
                } else {
                    connect({clientSocket, nullptr});

                    struct kevent event {};
                    EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
                    ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
                }
            } else if (eventSet[i].filter == EVFILT_READ) {
                if (eventSet[i].flags & EV_EOF) {
                    if (sslContext) {
                        auto *clientSSL = (ssl_st *) eventSet[i].udata;
                        closing({(socket_t) eventSet[i].ident, clientSSL});
                        SSL_free(clientSSL);
                    } else {
                        closing({(socket_t) eventSet[i].ident, nullptr});
                    }
                    close((int) eventSet[i].ident);
                    continue;
                } else {
                    if (sslContext) {
                        auto clientSocket = (socket_t) eventSet[i].ident;

                        char buf;
                        auto rt = SSL_peek((ssl_st *) eventSet[i].udata, &buf, 1);
                        if (rt <= 0) {
                            closing({clientSocket, eventSet[i].udata});

                            SSL_free((ssl_st *) eventSet[i].udata);
                            ::close(clientSocket);

                            continue;
                        }
                    }

                    handle({(socket_t) eventSet[i].ident, eventSet[i].udata});
                    continue;
                }
            }
        }
    }
}

void *DarwinService::handshake(sese::socket_t client) noexcept {
    ssl_st *clientSSL = nullptr;
    clientSSL = SSL_new((SSL_CTX *) sslContext->getContext());
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

void DarwinService::connect(DarwinServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;
        onConnect(myCtx);
    });
}

void DarwinService::handle(DarwinServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;

        onHandle(myCtx);

        if (!myCtx.isClosing) {
            struct kevent event {};
            EV_SET(&event, myCtx.socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, myCtx.ssl);
            ::kevent(kqueue, &event, 1, nullptr, 0, nullptr);
        } else {
            onClosing(myCtx);
            if (sslContext) {
                SSL_free((ssl_st *) myCtx.ssl);
            }
            close(myCtx.socket);
        }
    });
}

void DarwinService::closing(DarwinServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;
        onClosing(myCtx);
    });
}

void DarwinService::start() noexcept {
    mainThread = std::make_unique<Thread>([this]() { loop(); }, "LINUX_MAIN");
    threadPool = std::make_unique<ThreadPool>("LINUX_SERV", threads);
    mainThread->start();
}

void DarwinService::shutdown() noexcept {
    if (mainThread != nullptr && mainThread->joinable()) {
        exitStatus = true;
        mainThread->join();
        threadPool->shutdown();
        close(socket);
        close(kqueue);
    }
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif