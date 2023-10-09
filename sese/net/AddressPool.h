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
template<class Address>
class API AddressPool final {
public:
    /// 获取域名与 IP 地址的映射关系
    /// \param domain 域名
    /// \retval nullptr 未命中缓存且查找失败
    /// \retval other 查找到地址
    static std::shared_ptr<Address> lookup(const std::string &domain) noexcept;

private:
    AddressPool() = default;

    static AddressPool pool;

    std::map<std::string, std::shared_ptr<Address>> addressMap;
};

template<class Address>
AddressPool<Address> AddressPool<Address>::pool;

using IPv4AddressPool = AddressPool<sese::net::IPv4Address>;
using IPv6AddressPool = AddressPool<sese::net::IPv6Address>;
} // namespace sese::net


template<class Address>
std::shared_ptr<Address> sese::net::AddressPool<Address>::lookup(const std::string &domain) noexcept {
    auto iterator = pool.addressMap.find(domain);
    auto inet = std::is_same<Address, sese::net::IPv4Address>::value ? AF_INET : AF_INET6;
    if (iterator == pool.addressMap.end()) {
        auto address = Address::lookUpAny(domain, inet, IPPROTO_IP);
        if (nullptr == address) {
            // 未命中缓存且查找失败
            return nullptr;
        } else {
            auto ip = dynamicPointerCast<Address>(address);
            pool.addressMap[domain] = ip;
            return ip;
        }
    } else {
        return iterator->second;
    }
}