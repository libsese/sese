#include "sese/net/Socket.h"
#include "sese/util/Util.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

using namespace sese::net;

Socket::Socket(Family family, Type type, int32_t protocol) noexcept {
    handle = ::socket((int32_t) family, (int32_t) type, protocol);
}

Socket::~Socket() noexcept { // NOLINT
}

int32_t Socket::bind(Address::Ptr addr) noexcept {
    address = std::move(addr);
    return ::bind(handle, address->getRawAddress(), address->getRawAddressLength());
}

int32_t Socket::connect(Address::Ptr addr) noexcept {
    address = std::move(addr);
    while (true) {
        auto rt = ::connect(handle, address->getRawAddress(), address->getRawAddressLength());
        if (rt != 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EALREADY || err == EINPROGRESS) {
                sese::sleep(0);
                continue;
            } else if (err == EISCONN) {
                return 0;
            } else {
                return rt;
            }
        }
    }
}

int32_t Socket::listen(int32_t backlog) const noexcept {
    return ::listen(handle, backlog);
}

Socket::Ptr Socket::accept() const {
    if (address->getRawAddress()->sa_family == AF_INET) {
        sockaddr addr{0};
        socklen_t addrLen = sizeof(addr);
        auto clientHandle = ::accept(handle, (sockaddr *) &addr, &addrLen);
        auto pAddr = Address::create((sockaddr *) &addr, addrLen);
        auto pClientSocket = new Socket(clientHandle, pAddr);
        return std::shared_ptr<Socket>(pClientSocket);
    } else {
        sockaddr_in6 addr{0};
        socklen_t addrLen = sizeof(addr);
        auto clientHandle = ::accept(handle, (sockaddr *) &addr, &addrLen);
        auto pAddr = Address::create((sockaddr *) &addr, addrLen);
        auto pClientSocket = new Socket(clientHandle, pAddr);
        return std::shared_ptr<Socket>(pClientSocket);
    }
}

int32_t Socket::shutdown(ShutdownMode mode) const {
    return ::shutdown(handle, (int32_t) mode);
}

bool Socket::setNonblocking() const noexcept {
    int32_t opt = fcntl(handle, F_GETFL);
    if (opt == -1) return false;
    return fcntl(handle, F_SETFL, opt | O_NONBLOCK) == 0;
}

int64_t Socket::read(void *buffer, size_t length) {
    return ::read(handle, buffer, length);
}

int64_t Socket::write(const void *buffer, size_t length) {
    return ::write(handle, buffer, length);
}

int64_t Socket::send(void *buffer, size_t length, const IPAddress::Ptr &to, int32_t flags) const {
    return ::sendto(handle, buffer, length, flags, to->getRawAddress(), to->getRawAddressLength());
}

int64_t Socket::recv(void *buffer, size_t length, const IPAddress::Ptr &from, int32_t flags) const {
    sockaddr *addr = nullptr;
    socklen_t len = 0;
    if (from) {
        len = from->getRawAddressLength();
        addr = from->getRawAddress();
    }
    return ::recvfrom(handle, buffer, length, flags, addr, &len);
}

void Socket::close() {
    ::close(handle);
}

Socket::Socket(socket_t handle, Address::Ptr address) noexcept {
    this->handle = handle;
    this->address = std::move(address);
}

int64_t Socket::peek(void *buffer, size_t length) {
    return ::recv(handle, buffer, length, MSG_PEEK);
}

int64_t Socket::trunc(size_t length) {
    return ::recv(handle, nullptr, length, MSG_TRUNC);
}