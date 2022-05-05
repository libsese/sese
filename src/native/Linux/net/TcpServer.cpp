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

        if(len <= 0) {
            return -1;
        } else {
            transfer += len;
            size -= len;
            if(0 == size) {
                break;
            }
        }

        count += 1;
        if(count == 64) {
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

        if(len <= 0) {
            return -1;
        } else {
            transfer += len;
            size -= len;
            if(0 == size) {
                break;
            }
        }

        count += 1;
        if(count == 64) {
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

bool TcpServer::init(const IPAddress::Ptr &ipAddress, size_t threads) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t serverSocket = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1) {
        return false;
    }

    socket = std::make_shared<Socket>(serverSocket, ipAddress);

    if (!socket->setNonblocking(true)) {
        return false;
    }

    if (-1 == socket->bind(ipAddress)) {
        socket->close();
        return false;
    }

    if (-1 == socket->listen(SERVER_MAX_CONNECTION)) {
        socket->close();
        return false;
    }

    epollFd = epoll_create1(0);
    if (-1 == epollFd) {
        socket->close();
        return false;
    }

    epoll_event event{};
    event.data.fd = serverSocket;
    event.events = EPOLLIN;

    if (-1 == epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event)) {
        socket->close();
        return false;
    }

    threadPool = std::make_shared<ThreadPool>("TcpServer", threads);
    isShutdown = false;
    return true;
}

void TcpServer::loopWith(const std::function<void(IOContext *)> &handler) {
    Socket::Ptr client;

    while (!isShutdown) {
        int32_t nfds = epoll_wait(epollFd, events, 64, 1000);
        if (-1 == nfds) {
            continue;
        }

        for (int32_t n = 0; n < nfds; n++) {
            if (events[n].data.fd == socket->getRawSocket()) {
                client = socket->accept();
                if (client == nullptr) {
                    continue;
                }

                if (!client->setNonblocking(true)) {
                    socket->close();
                }

                epoll_event event{};
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client->getRawSocket();
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client->getRawSocket(), &event) == -1) {
                    client->close();
                    continue;
                }
            } else {
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