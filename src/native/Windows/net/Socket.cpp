#include "net/Socket.h"

sese::Socket::Socket(Family family, Type type, int32_t protocol) noexcept {
    handle = socket((int32_t) family, (int32_t) type, protocol);
}

sese::Socket::~Socket() noexcept {
    closesocket(handle);
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
    sockaddr addr{0};
    int32_t addrLen;
    if (address->getRawAddress()->sa_family == AF_INET) {
        addrLen = sizeof(sockaddr_in);
    } else {
        addrLen = sizeof(sockaddr_in6);
    }
    socket_t clientHandle = ::accept(handle, &addr, &addrLen);

    auto pAddr = Address::create(&addr, addrLen);
    auto pClientSocket = new Socket(clientHandle, pAddr);
    return std::shared_ptr<Socket>(pClientSocket);
}

int32_t sese::Socket::shutdown(ShutdownMode mode) const {
    return ::shutdown(handle, (int32_t) mode);
}

int64_t sese::Socket::read(void *buffer, size_t length) {
    return ::recv(handle, (char *) buffer, (int32_t) length, 0);
}

int64_t sese::Socket::write(void *buffer, size_t length) {
    return ::send(handle, (char *) buffer, (int32_t) length, 0);
}

int64_t sese::Socket::send(void *buffer, size_t length, const IPAddress::Ptr &to, int32_t flags) const {
    return sendto(handle, (char *) buffer, (int32_t) length, flags, to->getRawAddress(), to->getRawAddressLength());
}

int64_t sese::Socket::recv(void *buffer, size_t length, const IPAddress::Ptr &from, int32_t flags) const {
    int32_t len = from->getRawAddressLength();
    return recvfrom(handle, (char *) buffer, (int32_t) length, flags, from->getRawAddress(), &len);
}

void sese::Socket::close() {
    closesocket(handle);
}

sese::Socket::Socket(socket_t handle, Address::Ptr address) noexcept {
    this->handle = handle;
    this->address = std::move(address);
}
