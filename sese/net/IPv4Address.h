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
 * @file IPv4Address.h
 * @author kaoru
 * @brief IPv4 address class
 * @date April 4, 2022
 */

#pragma once

#include "sese/net/IPAddress.h"

namespace sese::net {

/**
 * @brief IPv4 address class
 */
class  IPv4Address final : public IPAddress {
public:
    using Ptr = std::shared_ptr<IPv4Address>;
    static IPv4Address::Ptr create(const char *address, uint16_t port);
    static IPv4Address::Ptr localhost(uint16_t port = 0);
    static IPv4Address::Ptr any(uint16_t port = 0);

public:
    explicit IPv4Address(const sockaddr_in &address);
    explicit IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

public:
    [[nodiscard]] sockaddr *getRawAddress() const noexcept override;
    [[nodiscard]] socklen_t getRawAddressLength() const noexcept override;
    [[nodiscard]] std::string getAddress() const noexcept override;

    [[nodiscard]] IPAddress::Ptr getBroadcastAddress(uint32_t prefix_len) const noexcept override;
    [[nodiscard]] IPAddress::Ptr getNetworkAddress(uint32_t prefix_len) const noexcept override;
    [[nodiscard]] IPAddress::Ptr getSubnetMask(uint32_t prefix_len) const noexcept override;

    void setPort(uint16_t port) noexcept override { address.sin_port = ToBigEndian16(port); }
    [[nodiscard]] uint16_t getPort() const noexcept override {
        //#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32)
        //            return ByteSwap16(address.sin_port);
        //#else
        //            return address.sin_port;
        //#endif
        return FromBigEndian16(address.sin_port);
    }

    void setFamily(uint16_t family) noexcept override {
        address.sin_family = family;
    }
    uint16_t getFamily() noexcept override {
        return address.sin_family;
    }

private:
    sockaddr_in address{0};
};

} // namespace sese::net