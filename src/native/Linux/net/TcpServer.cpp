#include <fcntl.h>
#include <sese/net/TcpServer.h>

using sese::IOContext;
using sese::IPAddress;
using sese::Socket;
using sese::TcpServer;

IOContext::IOContext() noexcept {}

IOContext::~IOContext() noexcept {}

int64_t IOContext::recv(void *buffer, size_t size) noexcept {
    int32_t count = 0;
    int64_t transfer = 0;
    int64_t len;
    while (true) {
        if (size >= 1024) {
            len = socket->read(buffer, 1024);
        } else {
            len = socket->read(buffer, size);
        }

        if (len <= 0) {
            return -1;
        } else {
            transfer += len;
            size -= len;
            if (0 == size) {
                break;
            }
        }

        count += 1;
        if (count == 64) {
            return transfer;
        }
    }
    return transfer;
}

int64_t IOContext::send(const void *buffer, size_t size) noexcept {
    int32_t count = 0;
    int64_t transfer = 0;
    int64_t len;
    while (true) {
        if (size >= 1024) {
            len = socket->write(buffer, 1024);
        } else {
            len = socket->write(buffer, size);
        }

        if (len <= 0) {
            return -1;
        } else {
            transfer += len;
            size -= len;
            if (0 == size) {
                break;
            }
        }

        count += 1;
        if (count == 64) {
            return transfer;
        }
    }
    return transfer;
}

int32_t IOContext::shutdown(Socket::ShutdownMode mode) const noexcept {
    return socket->shutdown(mode);
}

void IOContext::close() const noexcept {
    socket->close();
}

const IPAddress::Ptr &IOContext::getClientAddress() const noexcept {
    return reinterpret_cast<const IPAddress::Ptr &>(socket->getAddress());
}

TcpServer::Ptr TcpServer::create(const IPAddress::Ptr &ipAddress, size_t threads) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t serverSocket = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1) {
        return nullptr;
    }

    auto server = std::unique_ptr<TcpServer>(new TcpServer);
    server->socket = std::make_shared<Socket>(serverSocket, ipAddress);

    if (!server->socket->setNonblocking(true)) {
        return nullptr;
    }

    if (-1 == server->socket->bind(ipAddress)) {
        server->socket->close();
        return nullptr;
    }

    if (-1 == server->socket->listen(SERVER_MAX_CONNECTION)) {
        server->socket->close();
        return nullptr;
    }

    server->epollFd = epoll_create1(0);
    if (-1 == server->epollFd) {
        server->socket->close();
        return nullptr;
    }

    epoll_event event{};
    event.data.fd = serverSocket;
    event.events = EPOLLIN;

    if (-1 == epoll_ctl(server->epollFd, EPOLL_CTL_ADD, serverSocket, &event)) {
        server->socket->close();
        return nullptr;
    }

    server->threadPool = std::make_unique<ThreadPool>("TcpServer", threads);
    server->isShutdown = false;
    return server;
}

void TcpServer::loopWith(const std::function<void(IOContext *)> &handler) {
    while (!isShutdown) {
        int32_t nfds = epoll_wait(epollFd, events, 64, 1000);
        if (-1 == nfds) {
            continue;
        }

        for (int32_t n = 0; n < nfds; n++) {
            if (events[n].data.fd == socket->getRawSocket()) {
                socket_t client = accept(socket->getRawSocket(), nullptr, nullptr);
                if (client == -1) {
                    continue;
                }

                int32_t opt = fcntl(client, F_GETFL);
                if (opt == -1) {
                    close(client);
                    continue;
                }
                if (fcntl(client, F_SETFL, opt | O_NONBLOCK) != 0) {
                    close(client);
                    continue;
                }

                epoll_event event{};
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client, &event) == -1) {
                    close(client);
                    continue;
                }
            } else {
                socket_t fd = events[n].data.fd;
                sockaddr_storage sockaddrStorage{};
                socklen_t socklen = sizeof(sockaddrStorage);
                getpeername(fd, reinterpret_cast<struct sockaddr *>(&sockaddrStorage), &socklen);
                Address::Ptr address = Address::create(reinterpret_cast<struct sockaddr *>(&sockaddrStorage), socklen);

                Socket::Ptr client = std::make_shared<Socket>(fd, address);
                auto *ioContext = new IOContext;
                ioContext->socket = client;
                threadPool->postTask([handler, ioContext]() {
                    handler(ioContext);
                    delete ioContext;
                });
            }
        }
    }
}

void TcpServer::shutdown() {
    isShutdown = true;
    threadPool->shutdown();
    close(epollFd);
    socket->close();
}