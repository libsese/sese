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
 * @file IPAddress.h
 * @author kaoru
 * @brief IP 地址类
 * @date 2022年4月4日
 */
#pragma once

#include "sese/net/Address.h"
#include "sese/util/Endian.h"

namespace sese::net {

/**
 * @brief IP 地址类
 */
class  IPAddress : public Address {
public:
    using Ptr = std::shared_ptr<IPAddress>;

    static Ptr create(const char *address, uint16_t port = 0);

public:
    [[nodiscard]] virtual Ptr getBroadcastAddress(uint32_t prefix_len) const noexcept = 0;
    [[nodiscard]] virtual Ptr getNetworkAddress(uint32_t prefix_len) const noexcept = 0;
    [[nodiscard]] virtual Ptr getSubnetMask(uint32_t prefix_len) const noexcept = 0;
    virtual void setPort(uint16_t port) noexcept = 0;
    [[nodiscard]] virtual uint16_t getPort() const noexcept = 0;
    virtual void setFamily(uint16_t family) noexcept = 0;
    [[nodiscard]] virtual uint16_t getFamily() noexcept = 0;
};

} // namespace sese::net