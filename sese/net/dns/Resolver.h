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

/// @file Resolver.h
/// @brief 域名解析器
/// @author kaoru
/// @date 2024年10月30日

#pragma once

#include <sese/net/IPv6Address.h>

#include <vector>
#include <random>

namespace sese::net::dns {
/// \brief 域名解析器
class Resolver {
    std::vector<IPAddress::Ptr> name_servers;

    std::random_device device;
    std::mt19937 generator;

    std::vector<IPAddress::Ptr> resolve(const IPAddress::Ptr &name_server, const std::string &hostname, uint16_t type);
public:
    Resolver();

    /// 添加 NameServer
    /// @param ip IP
    /// @param port 端口
    /// @return 是否添加成功，这取决于填写的IP地址格式
    bool addNameServer(const std::string &ip, uint16_t port = 53);

    /// 添加 NameServer
    /// @param ip_address IP
    void addNameServer(const IPAddress::Ptr &ip_address);

    /// 解析域名
    /// @param hostname 域名
    /// @param type 类型 (sese::net::dns::TYPE_A || sese::net::dns::TYPE_AAAA)
    /// @return 解析结果
    std::vector<IPAddress::Ptr> resolve(const std::string &hostname, uint16_t type);
};
} // namespace sese::net::dns