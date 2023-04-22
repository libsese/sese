#pragma once

#include "sese/net/IPv6Address.h"

#include <array>

namespace sese::system {

    struct NetworkInterface {
        std::string name;
        std::vector<sese::net::IPv4Address::Ptr> ipv4Addresses;
        std::vector<sese::net::IPv6Address::Ptr> ipv6Addresses;
        std::array<unsigned char, 6> mac;
    };

    class NetworkUtil {
    public:
        static std::vector<NetworkInterface> getNetworkInterface() noexcept;
    };
}