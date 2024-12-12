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
/// \date July 28, 2022
/// \version 0.1
/// \brief IP address pool

#pragma once

#include <sese/net/IPv6Address.h>

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net {

/// IP address pool
template<class ADDRESS>
class  AddressPool final {
public:
    /// Get the mapping between domain name and IP address
    /// \param domain The domain name
    /// \retval nullptr Not found in cache and lookup failed
    /// \retval other Address found
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
            // The cache was missed and the lookup failed
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