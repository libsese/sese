#include "net/Socket.h"
#include <unistd.h>

sese::Socket::Socket(Family family, Type type, int32_t protocol) noexcept {
    handle = ::socket((int32_t) family, (int32_t) type, protocol);
}

sese::Socket::~Socket() noexcept {
    ::close(handle);
}

int32_t sese::Socket::bind(Address::Ptr addr) noexcept {
    address = std::move(addr);
    return ::bind(handle, address->getRawAddress(), address->getRawAddressLength());
}

int32_t sese::Socket::connect(Address::Ptr addr) noexcept {
    address = std::move(addr);
    return ::connect(handle, address->getRawAddress(), address->getRawAddressLength());
}

int32_t sese::Socket::listen(int32_t backlog) const noexcept {
    return ::listen(handle, backlog);
}

sese::Socket::Ptr sese::Socket::accept() const {
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

int32_t sese::Socket::shutdown(ShutdownMode mode) const {
    return ::shutdown(handle, (int32_t) mode);
}

int64_t sese::Socket::read(void *buffer, size_t length) {
    return ::read(handle, buffer, length);
}

int64_t sese::Socket::write(void *buffer, size_t length) {
    return ::write(handle, buffer, length);
}

int64_t sese::Socket::send(void *buffer, size_t length, const IPAddress::Ptr &to, int32_t flags) const {
    return ::sendto(handle, buffer, length, flags, to->getRawAddress(), to->getRawAddressLength());
}

int64_t sese::Socket::recv(void *buffer, size_t length, const IPAddress::Ptr &from, int32_t flags) const {
    auto len = from->getRawAddressLength();
    return ::recvfrom(handle, buffer, length, flags, from->getRawAddress(), &len);
}

void sese::Socket::close() {
    ::close(handle);
}

sese::Socket::Socket(socket_t handle, Address::Ptr address) noexcept {
    this->handle = handle;
    this->address = std::move(address);
}

int32_t sese::Socket::setOption(Option option, int32_t &value) const {
    return ::setsockopt(handle, SOL_SOCKET, (int32_t)option, (const void *)&value, sizeof(value));
}

int32_t sese::Socket::getOption(Option option, int32_t &value) const {
    socklen_t size = sizeof(value);
    return ::getsockopt(handle, SOL_SOCKET, (int32_t)option, (void *)&value, &size);
}