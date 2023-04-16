#include "sese/net/V2Server.h"
#include "sese/util/Util.h"
#include "openssl/ssl.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace sese::net::v2;

inline static int setNonblocking(socket_t socket) {
    auto option = fcntl(socket, F_GETFL);
    if (option != -1) {
        return fcntl(socket, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

LinuxServiceIOContext::LinuxServiceIOContext(socket_t socket, void *ssl) noexcept
    : socket(socket),
      ssl(ssl) {
}

int64_t LinuxServiceIOContext::peek(void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_peek((ssl_st *) ssl, (char *) buf, (int) len);
    } else {
        return ::recv(socket, (char *) buf, (int) len, MSG_PEEK);
    }
}

int64_t LinuxServiceIOContext::read(void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_read((ssl_st *) ssl, (char *) buf, (int) len);
    } else {
        return ::recv(socket, (char *) buf, (int) len, 0);
    }
}

int64_t LinuxServiceIOContext::write(const void *buf, size_t len) noexcept {
    if (ssl) {
        return ::SSL_write((ssl_st *) ssl, (const char *) buf, (int) len);
    } else {
        return ::send(socket, (const char *) buf, (int) len, 0);
    }
}

void LinuxServiceIOContext::close() noexcept {
    if (ssl) {
        ::SSL_shutdown((ssl_st *) ssl);
        ::shutdown(socket, SHUT_RDWR);
    } else {
        ::shutdown(socket, SHUT_RDWR);
    }
    isClosing = true;
}

LinuxService::Ptr LinuxService::create(ServerOption *opt) noexcept {
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

    if (-1 == bind(sock, opt->address->getRawAddress(), opt->address->getRawAddressLength())) {
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

    return std::unique_ptr<LinuxService>(server);
}

void LinuxService::loop() noexcept {
    while (true) {
        if (exit) break;

        int numOfFds = epoll_wait(epoll, eventSet, MaxEventSize, 0);
        if (-1 == numOfFds) continue;

        for (int i = 0; i < numOfFds; ++i) {
            auto fd = eventSet[i].data.fd;
            if (eventSet[i].data.fd == socket) {
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

                    connect({clientSocket, clientSSL});

                    epoll_event event{
                            .events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT,
                            .data = {.ptr = clientSSL}};
                    epoll_ctl(epoll, EPOLL_CTL_ADD, clientSocket, &event);
                } else {

                    connect({clientSocket, nullptr});

                    epoll_event event{
                            .events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT,
                            .data = {.fd = clientSocket}};
                    epoll_ctl(epoll, EPOLL_CTL_ADD, clientSocket, &event);
                }
            } else if (eventSet[i].events & EPOLLIN) {
                if (eventSet[i].events & EPOLLRDHUP || eventSet[i].events & EPOLLHUP) {
                    // 仅有对端关闭能触发
                    socket_t clientSocket;
                    if (option->isSSL) {
                        clientSocket = SSL_get_fd((ssl_st *) eventSet[i].data.ptr);

                        closing({clientSocket, eventSet[i].data.ptr});
                    } else {
                        clientSocket = eventSet[i].data.fd;

                        closing({clientSocket, nullptr});
                    }
                    ::close(clientSocket);
                } else {
                    if (option->isSSL) {
                        char buf;
                        auto rt = SSL_peek((ssl_st *) eventSet[i].data.ptr, &buf, 1);
                        if (rt <= 0) {
                            continue;
                        }

                        auto clientSocket = SSL_get_fd((ssl_st *) eventSet[i].data.ptr);
                        handle({clientSocket, eventSet[i].data.ptr});
                    } else {
                        handle({eventSet[i].data.fd, nullptr});
                    }
                }
            }
        }
    }
}

void *LinuxService::handshake(socket_t client) noexcept {
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

void LinuxService::connect(sese::net::v2::LinuxServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;
        option->onConnect(myCtx);
    });
}

void LinuxService::handle(sese::net::v2::LinuxServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;
        option->onHandle(myCtx);
        if (!myCtx.isClosing) {
            if (option->isSSL) {
                epoll_event event{
                        .events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT,
                        .data = {.ptr = myCtx.ssl}};
                epoll_ctl(epoll, EPOLL_CTL_MOD, myCtx.socket, &event);
            } else {
                epoll_event event{
                        .events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT,
                        .data = {.fd = myCtx.socket}};
                epoll_ctl(epoll, EPOLL_CTL_MOD, myCtx.socket, &event);
            }
        } else {
            // 此处为主动关闭触发
            option->onClosing(myCtx);
        }
    });
}

void LinuxService::closing(sese::net::v2::LinuxServiceIOContext ctx) noexcept {
    threadPool->postTask([ctx, this]() {
        auto myCtx = ctx;
        option->onClosing(myCtx);
    });
}

void LinuxService::start() noexcept {
    mainThread = std::make_unique<Thread>([this]() { loop(); }, "LINUX_MAIN");
    threadPool = std::make_unique<ThreadPool>("LINUX_SERV", option->threads);
    mainThread->start();
}

void LinuxService::shutdown() noexcept {
    if (mainThread != nullptr && mainThread->joinable()) {
        exit = true;
        mainThread->join();
        threadPool->shutdown();
        close(socket);
        close(epoll);
    }
}