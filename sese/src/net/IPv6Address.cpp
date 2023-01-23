#include "sese/net/IPv6Address.h"
//#include "system/Environment.h"
#include <cstring>

#ifdef _WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#endif

sese::IPv6Address::Ptr sese::IPv6Address::create(const char *address, uint16_t port) {
    IPv6Address::Ptr result(new IPv6Address);
    auto err = inet_pton(AF_INET6, address, &result->address.sin6_addr);
    result->address.sin6_family = AF_INET6;
    result->address.sin6_port = ToBigEndian16(port);
    if (err <= 0) {
        return nullptr;
    } else {
        return result;
    }
}

sese::IPv6Address::IPv6Address() noexcept {
    this->address.sin6_family = AF_INET6;
}

sese::IPv6Address::IPv6Address(const sockaddr_in6 &address) noexcept : address(address) {}

sese::IPv6Address::IPv6Address(uint8_t *address, uint16_t port) {
    this->address.sin6_family = AF_INET6;
    //    if (Environment::isLittleEndian()) {
    //        this->address.sin6_port = ByteSwap16(port);
    //    } else {
    //        this->address.sin6_port = port;
    //    }
    this->address.sin6_port = ToBigEndian16<uint16_t>(port);
    memcpy(&this->address.sin6_addr.s6_addr, address, 16);
}

sockaddr *sese::IPv6Address::getRawAddress() const noexcept {
    return (sockaddr *) &this->address;
}

socklen_t sese::IPv6Address::getRawAddressLength() const noexcept {
    return sizeof(this->address);
}

std::string sese::IPv6Address::getAddress() const noexcept {
    auto *addr = (uint16_t *) &this->address.sin6_addr.s6_addr;
    char temp[40];
    sprintf(temp,
            "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
            ToBigEndian16(addr[0]),
            ToBigEndian16(addr[1]),
            ToBigEndian16(addr[2]),
            ToBigEndian16(addr[3]),
            ToBigEndian16(addr[4]),
            ToBigEndian16(addr[5]),
            ToBigEndian16(addr[6]),
            ToBigEndian16(addr[7]));
    return {temp};
}

sese::IPAddress::Ptr sese::IPv6Address::getBroadcastAddress(uint32_t prefixLen) const noexcept {
    sockaddr_in6 addr(this->address);
    addr.sin6_addr.s6_addr[prefixLen / 8] |= CreateMask<uint8_t>(prefixLen % 8);
    for (auto i = prefixLen / 8 + 1; i < 16; i++) {
        addr.sin6_addr.s6_addr[i] = 0xFF;
    }
    return std::make_shared<IPv6Address>(addr);
}

sese::IPAddress::Ptr sese::IPv6Address::getNetworkAddress(uint32_t prefixLen) const noexcept {
    sockaddr_in6 addr(this->address);
    addr.sin6_addr.s6_addr[prefixLen / 8] &= CreateMask<uint8_t>(prefixLen % 8);
    for (auto i = prefixLen / 8 + 1; i < 16; ++i) {
        addr.sin6_addr.s6_addr[i] = 0x00;
    }
    return std::make_shared<IPv6Address>(addr);
}

sese::IPAddress::Ptr sese::IPv6Address::getSubnetMask(uint32_t prefixLen) const noexcept {
    sockaddr_in6 subnet{0};
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefixLen / 8] = ~CreateMask<uint8_t>(prefixLen % 8);
    for (auto i = 0; i < prefixLen / 8; ++i) {
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(subnet);
}
