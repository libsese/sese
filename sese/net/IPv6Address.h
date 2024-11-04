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

/**
 * @file IPv6Address.h
 * @author kaoru
 * @brief IPv6 地址类
 * @date 2022年4月4日
 */
#pragma once

#include "sese/net/IPv4Address.h"

namespace sese::net {

/**
 * @brief IPv6 地址类
 */
class  IPv6Address final : public IPAddress {
public:
    using Ptr = std::shared_ptr<IPv6Address>;
    static IPv6Address::Ptr create(const char *address, uint16_t port);
    static IPv6Address::Ptr localhost(uint16_t port = 0);
    static IPv6Address::Ptr any(uint16_t port = 0);

public:
    explicit IPv6Address() noexcept;
    explicit IPv6Address(const sockaddr_in6 &address) noexcept;
    explicit IPv6Address(const uint8_t *address, uint16_t port = 0);

    [[nodiscard]] sockaddr *getRawAddress() const noexcept override;
    [[nodiscard]] socklen_t getRawAddressLength() const noexcept override;
    [[nodiscard]] std::string getAddress() const noexcept override;

    [[nodiscard]] IPAddress::Ptr getBroadcastAddress(uint32_t prefix_len) const noexcept override;
    [[nodiscard]] IPAddress::Ptr getNetworkAddress(uint32_t prefix_len) const noexcept override;
    [[nodiscard]] IPAddress::Ptr getSubnetMask(uint32_t prefix_len) const noexcept override;

    void setPort(uint16_t port) noexcept override { this->address.sin6_port = ToBigEndian16(port); }
    [[nodiscard]] uint16_t getPort() const noexcept override { return FromBigEndian16(address.sin6_port); }

    void setFamily(uint16_t family) noexcept override {
        address.sin6_family = family;
    }
    uint16_t getFamily() noexcept override {
        return address.sin6_family;
    }

private:
    sockaddr_in6 address{0};
};
} // namespace sese::net