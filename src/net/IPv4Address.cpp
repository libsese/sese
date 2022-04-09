#include "net/IPv4Address.h"
//#include "system/Environment.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

sese::IPv4Address::Ptr sese::IPv4Address::create(const char *address, uint16_t port) {
    IPv4Address::Ptr result(new IPv4Address);
    //    if (Environment::isLittleEndian()) {
    //        result->address.sin_port = ByteSwap16(port);
    //    } else {
    //        result->address.sin_port = port;
    //    }
    result->address.sin_port = ToBigEndian16(port);
    int err = inet_pton(AF_INET, address, &result->address.sin_addr);
    if (err <= 0) {
        return nullptr;
    } else {
        return result;
    }
}

sese::IPv4Address::IPv4Address(const sockaddr_in &address) : address(address) {}

sese::IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
    this->address.sin_family = AF_INET;
    //    if (Environment::isLittleEndian()) {
    //        this->address.sin_port = ByteSwap16(port);
    //        this->address.sin_addr.s_addr = ByteSwap32(address);
    //    } else {
    //        this->address.sin_port = port;
    //        this->address.sin_addr.s_addr = address;
    //    }
    this->address.sin_port = ToBigEndian16(port);
    this->address.sin_addr.s_addr = ToBigEndian32(address);
}

sockaddr *sese::IPv4Address::getRawAddress() const noexcept {
    return (sockaddr *) &address;
}

socklen_t sese::IPv4Address::getRawAddressLength() const noexcept {
    return sizeof(this->address);
}

std::string sese::IPv4Address::getAddress() const noexcept {
    uint32_t addr = this->address.sin_addr.s_addr;
    //    if (!Environment::isLittleEndian()) {
    //        addr = ByteSwap32(addr);
    //    }
    addr = ToLittleEndian32(addr);

    char temp[16]{0};
    sprintf(temp,
            "%d.%d.%d.%d",
            (addr & 0x000000FF) >> 0,
            (addr & 0x0000FF00) >> 8,
            (addr & 0x00FF0000) >> 16,
            (addr & 0xFF000000) >> 24);
    return {temp};
}

sese::IPAddress::Ptr sese::IPv4Address::getBroadcastAddress(uint32_t prefixLen) const noexcept {
    if (prefixLen > 32) {
        return nullptr;
    }

    sockaddr_in addr(this->address);
    auto mask = CreateMask<uint32_t>(prefixLen);
    //    if (Environment::isLittleEndian()) {
    //        mask = ByteSwap32(mask);
    //    }
    mask = ToBigEndian32(mask);
    addr.sin_addr.s_addr |= mask;
    return std::make_shared<IPv4Address>(addr);
}

sese::IPAddress::Ptr sese::IPv4Address::getNetworkAddress(uint32_t prefixLen) const noexcept {
    if (prefixLen > 32) {
        return nullptr;
    }

    sockaddr_in addr(this->address);
    auto mask = CreateMask<uint32_t>(prefixLen);
    //    if (Environment::isLittleEndian()) {
    //        mask = ByteSwap32(mask);
    //    }
    mask = ToBigEndian32(mask);
    addr.sin_addr.s_addr &= mask;
    return std::make_shared<IPv4Address>(addr);
}

sese::IPAddress::Ptr sese::IPv4Address::getSubnetMask(uint32_t prefixLen) const noexcept {
    if (prefixLen > 32) {
        return nullptr;
    }

    sockaddr_in addr{0};
    addr.sin_family = AF_INET;
    auto mask = CreateMask<uint32_t>(prefixLen);
    //    if (Environment::isLittleEndian()) {
    //        mask = ByteSwap32(mask);
    //    }
    mask = ToBigEndian32(mask);
    addr.sin_addr.s_addr = ~mask;
    return std::make_shared<IPv4Address>(addr);
}
