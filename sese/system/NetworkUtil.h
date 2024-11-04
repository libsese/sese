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
* @file NetworkUtil.h
* @author kaoru
* @version 0.1
* @brief 网络工具类
* @date 2023年9月13日
*/

#pragma once

#include "sese/net/IPv6Address.h"

#include <array>

namespace sese::system {

/// 网络接口信息
struct NetworkInterface {
    std::string name;
    std::vector<sese::net::IPv4Address::Ptr> ipv4Addresses;
    std::vector<sese::net::IPv6Address::Ptr> ipv6Addresses;
    std::array<unsigned char, 6> mac;
};

/// 网络工具类
class NetworkUtil {
public:
    static std::vector<NetworkInterface> getNetworkInterface() noexcept;
};
} // namespace sese::system