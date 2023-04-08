#pragma once

#include "sese/net/IPv6Address.h"

#include <array>

namespace sese {

    struct NetworkInterface {
        std::string name;
        std::vector<IPv4Address::Ptr> ipv4Addresses;
        std::vector<IPv6Address::Ptr> ipv6Addresses;
        std::array<unsigned char, 6> mac;
    };

    class NetworkUtil {
    public:
        static std::vector<NetworkInterface> getNetworkInterface() noexcept;
    };
}