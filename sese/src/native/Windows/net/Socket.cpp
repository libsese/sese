#include "sese/net/Socket.h"

using namespace sese::net;

int32_t SocketInitiateTask::init() noexcept {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int32_t SocketInitiateTask::destroy() noexcept {
    return WSACleanup();
}

Socket::Socket(Family family, Type type, int32_t protocol) noexcept {
    handle = socket((int32_t) family, (int32_t) type, protocol);
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
    unsigned long ul = 1;
    return ioctlsocket(handle, FIONBIO, &ul) == 0;
}

int64_t Socket::read(void *buffer, size_t length) {
    return ::recv(handle, (char *) buffer, (int32_t) length, 0);
}

int64_t Socket::write(const void *buffer, size_t length) {
    return ::send(handle, (char *) buffer, (int32_t) length, 0);
}

int64_t Socket::send(void *buffer, size_t length, IPAddress::Ptr to, int32_t flags) const {
    return sendto(handle, (char *) buffer, (int32_t) length, flags, to->getRawAddress(), to->getRawAddressLength());
}

int64_t Socket::recv(void *buffer, size_t length, IPAddress::Ptr from, int32_t flags) const {
    int32_t len = from->getRawAddressLength();
    return recvfrom(handle, (char *) buffer, (int32_t) length, flags, from->getRawAddress(), &len);
}

void Socket::close() {
    closesocket(handle);
}

Socket::Socket(socket_t handle, Address::Ptr address) noexcept {
    this->handle = handle;
    this->address = std::move(address);
}

int64_t Socket::peek(void *buffer, size_t length) {
    return ::recv(handle, (char *) buffer, (int) length, MSG_PEEK);
}

int64_t Socket::trunc(size_t length) {
    return ::recv(handle, nullptr, (int) length, MSG_TRUNC);
}