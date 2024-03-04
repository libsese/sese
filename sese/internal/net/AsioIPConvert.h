#pragma once

#include "sese/net/IPv6Address.h"
#include <asio.hpp>

namespace sese::internal::net {
inline asio::ip::address convert(const sese::net::IPAddress::Ptr &addr) {
    if (addr->getRawAddress()->sa_family == AF_INET) {
        const auto ipv4 = reinterpret_cast<sockaddr_in *>(addr->getRawAddress());
        return asio::ip::make_address_v4(htonl(ipv4->sin_addr.s_addr));
    } else {
        const auto ipv6 = reinterpret_cast<sockaddr_in6 *>(addr->getRawAddress());
        const std::array<unsigned char, 16> bytes = {
                ipv6->sin6_addr.s6_addr[0],
                ipv6->sin6_addr.s6_addr[1],
                ipv6->sin6_addr.s6_addr[2],
                ipv6->sin6_addr.s6_addr[3],
                ipv6->sin6_addr.s6_addr[4],
                ipv6->sin6_addr.s6_addr[5],
                ipv6->sin6_addr.s6_addr[6],
                ipv6->sin6_addr.s6_addr[7],
                ipv6->sin6_addr.s6_addr[8],
                ipv6->sin6_addr.s6_addr[9],
                ipv6->sin6_addr.s6_addr[10],
                ipv6->sin6_addr.s6_addr[11],
                ipv6->sin6_addr.s6_addr[12],
                ipv6->sin6_addr.s6_addr[13],
                ipv6->sin6_addr.s6_addr[14],
                ipv6->sin6_addr.s6_addr[15]
        };
        return asio::ip::make_address_v6(bytes, ipv6->sin6_scope_id);
    }
}
} // namespace sese::internal::net
