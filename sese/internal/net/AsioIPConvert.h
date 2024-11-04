// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <sese/net/IPv6Address.h>
#include <asio.hpp>

// GCOVR_EXCL_START

namespace sese::internal::net {
inline asio::ip::address convert(const sese::net::IPAddress::Ptr &addr) {
    if (addr->getRawAddress()->sa_family == AF_INET) {
        const auto ipv4 = reinterpret_cast<sockaddr_in *>(addr->getRawAddress());
        return asio::ip::make_address_v4(htonl(ipv4->sin_addr.s_addr));
    }
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

template<typename EndpointType>
sese::net::IPAddress::Ptr convert(const EndpointType& endpoint) {
    static_assert(
        std::is_same_v<EndpointType, asio::ip::tcp::endpoint> || std::is_same_v<EndpointType, asio::ip::udp::endpoint>,
        "EndpointType must be either asio::ip::tcp::endpoint or asio::ip::udp::endpoint"
    );

    if (endpoint.address().is_v4()) {
        sockaddr_in sockaddr{};
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(endpoint.port());
        sockaddr.sin_addr.s_addr = htonl(endpoint.address().to_v4().to_uint());
        return std::make_shared<sese::net::IPv4Address>(sockaddr);
    }
    sockaddr_in6 sockaddr{};
    sockaddr.sin6_family = AF_INET6;
    sockaddr.sin6_port = htons(endpoint.port());
    auto bytes = endpoint.address().to_v6().to_bytes();
    memcpy(&sockaddr.sin6_addr, bytes.data(), bytes.size());
    return std::make_shared<sese::net::IPv6Address>(sockaddr);
}

} // namespace sese::internal::net

// GCOVR_EXCL_STOP
