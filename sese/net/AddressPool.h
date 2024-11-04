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

/// \file AddressPool.h
/// \author kaoru
/// \date 2022.07.28
/// \version 0.1
/// \brief IP 地址池

#pragma once

#include <sese/net/IPv6Address.h>

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net {

/// IP 地址池
template<class ADDRESS>
class  AddressPool final {
public:
    /// 获取域名与 IP 地址的映射关系
    /// \param domain 域名
    /// \retval nullptr 未命中缓存且查找失败
    /// \retval other 查找到地址
    static std::shared_ptr<ADDRESS> lookup(const std::string &domain) noexcept;

private:
    AddressPool() = default;

    static AddressPool pool;

    std::map<std::string, std::shared_ptr<ADDRESS>> addressMap;
};

template<class ADDRESS>
AddressPool<ADDRESS> AddressPool<ADDRESS>::pool;

using IPv4AddressPool = AddressPool<sese::net::IPv4Address>;
using IPv6AddressPool = AddressPool<sese::net::IPv6Address>;
} // namespace sese::net


template<class ADDRESS>
std::shared_ptr<ADDRESS> sese::net::AddressPool<ADDRESS>::lookup(const std::string &domain) noexcept {
    auto iterator = pool.addressMap.find(domain);
    auto inet = std::is_same<ADDRESS, sese::net::IPv4Address>::value ? AF_INET : AF_INET6;
    if (iterator == pool.addressMap.end()) {
        auto address = ADDRESS::lookUpAny(domain, inet, IPPROTO_IP);
        if (nullptr == address) {
            // 未命中缓存且查找失败
            return nullptr;
        } else {
            auto ip = dynamicPointerCast<ADDRESS>(address);
            pool.addressMap[domain] = ip;
            return ip;
        }
    } else {
        return iterator->second;
    }
}