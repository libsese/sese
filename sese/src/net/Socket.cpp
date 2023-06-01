#include <sese/net/Socket.h>

#ifdef SESE_PLATFORM_WINDOWS

int sese::net::Socket::setNonblocking(socket_t socket) noexcept {
    unsigned long ul = 1;
    return ioctlsocket(socket, FIONBIO, &ul);
}

void sese::net::Socket::closeSocket(socket_t socket) noexcept {
    closesocket(socket);
}

#else

#include <unistd.h>
#include <fcntl.h>

int sese::net::Socket::setNonblocking(socket_t socket) noexcept {
    auto option = fcntl(fd, F_GETFL);
    if (option != -1) {
        return fcntl(fd, F_SETFL, option | O_NONBLOCK);
    } else {
        return -1;
    }
}

void sese::net::Socket::closeSocket(socket_t socket) noexcept {
    ::close(socket);
}

#endif