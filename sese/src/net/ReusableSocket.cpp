#include "sese/net/ReusableSocket.h"

using namespace sese::net;

ReusableSocket::ReusableSocket(IPAddress::Ptr address, Socket::Type type)
    : addr(std::move(address)),
      type(type) {
}

std::optional<Socket> ReusableSocket::builtinMakeSocket() noexcept {
#ifdef SESE_PLATFORM_WINDOWS
    auto socket = Socket(
            addr->getRawAddress()->sa_family == AF_INET ? Socket::Family::IPv4 : Socket::Family::IPv6,
            type, IPPROTO_IP
    );

    BOOL opt = TRUE;
    if (0 != setsockopt(socket.getRawSocket(), SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt))) {
        return std::nullopt;
    }

    if (0 != socket.bind(addr)) {
        return std::nullopt;
    }

    return socket;
#else
    auto socket = Socket(
            addr->getRawAddress()->sa_family == AF_INET ? Socket::Family::IPv4 : Socket::Family::IPv6,
            type, IPPROTO_IP
    );

    int opt = 1;
    if (0 != setsockopt(socket.getRawSocket(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        return std::nullopt;
    }

    if (0 != setsockopt(socket.getRawSocket(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        return std::nullopt;
    }

    if (0 != socket.bind(addr)) {
        return std::nullopt;
    }

    return socket;
#endif
}

socket_t ReusableSocket::makeRawSocket() noexcept {
    auto socket = ReusableSocket::builtinMakeSocket();
    if (socket == std::nullopt) {
        return -1;
    }

    return socket.value().getRawSocket();
}

std::optional<Socket> ReusableSocket::makeSocket() noexcept {
    return ReusableSocket::builtinMakeSocket();
}