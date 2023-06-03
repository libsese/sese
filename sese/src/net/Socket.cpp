#include <sese/net/Socket.h>

socket_t sese::net::Socket::socket(int family, int type, int protocol) noexcept {
    return ::socket(family, type, protocol);
}

int sese::net::Socket::listen(socket_t socket, int backlog) noexcept {
    return ::listen(socket, backlog);
}

#ifdef SESE_PLATFORM_WINDOWS

int sese::net::Socket::setNonblocking(socket_t socket) noexcept {
    unsigned long ul = 1;
    return ioctlsocket(socket, FIONBIO, &ul);
}

void sese::net::Socket::close(socket_t socket) noexcept {
    closesocket(socket);
}

#else

#include <unistd.h>
#include <fcntl.h>

size_t sese::net::Socket::write(socket_t socket, const void *buffer, size_t len, int flags) noexcept {
    return ::send(socket, buffer, len, flags);
}

size_t sese::net::Socket::read(socket_t socket, void *buffer, size_t len, int flags) noexcept {
    return ::recv(socket, buffer, len, flags);
}

int sese::net::Socket::setNonblocking(socket_t socket) noexcept {
    auto option = fcntl(socket, F_GETFL);
    if (option != -1) {
        return fcntl(socket, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

void sese::net::Socket::close(socket_t socket) noexcept {
    ::close(socket);
}

#endif