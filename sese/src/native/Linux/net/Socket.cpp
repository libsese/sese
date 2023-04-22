#include "sese/net/Socket.h"
#include <unistd.h>
#include <fcntl.h>

using namespace sese::net;

Socket::Socket(Family family, Type type, int32_t protocol) noexcept {
    handle = ::socket((int32_t) family, (int32_t) type, protocol);
}

Socket::~Socket() noexcept {
}

int32_t Socket::bind(Address::Ptr addr) noexcept {
    address = std::move(addr);
    return ::bind(handle, address->getRawAddress(), address->getRawAddressLength());
}

int32_t Socket::connect(Address::Ptr addr) noexcept {
    address = std::move(addr);
    return ::connect(handle, address->getRawAddress(), address->getRawAddressLength());
}

int32_t Socket::listen(int32_t backlog) const noexcept {
    return ::listen(handle, backlog);
}

Socket::Ptr Socket::accept() const {
    if (address->getRawAddress()->sa_family == AF_INET) {
        sockaddr addr{0};
        socklen_t addrLen = sizeof(addr);
        auto clientHandle = ::accept(handle,(sockaddr *)&addr, &addrLen);
        auto pAddr = Address::create((sockaddr *)&addr, addrLen);
        auto pClientSocket = new Socket(clientHandle, pAddr);
        return std::shared_ptr<Socket>(pClientSocket);
    } else {
        sockaddr_in6 addr{0};
        socklen_t addrLen = sizeof(addr);
        auto clientHandle = ::accept(handle,(sockaddr *)&addr, &addrLen);
        auto pAddr = Address::create((sockaddr *)&addr, addrLen);
        auto pClientSocket = new Socket(clientHandle, pAddr);
        return std::shared_ptr<Socket>(pClientSocket);
    }
}

int32_t Socket::shutdown(ShutdownMode mode) const {
    return ::shutdown(handle, (int32_t) mode);
}

bool Socket::setNonblocking(bool enable) const noexcept {
    int32_t opt = fcntl(handle, F_GETFL);
    if(opt == -1) return false;
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
    auto len = from->getRawAddressLength();
    return ::recvfrom(handle, buffer, length, flags, from->getRawAddress(), &len);
}

void Socket::close() {
    ::close(handle);
}

Socket::Socket(socket_t handle, Address::Ptr address) noexcept {
    this->handle = handle;
    this->address = std::move(address);
}