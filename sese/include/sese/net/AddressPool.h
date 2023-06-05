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

    using IPv4AddressPool = AddressPool<sese::net::IPv4Address>;
    using IPv6AddressPool = AddressPool<sese::net::IPv6Address>;
}// namespace sese

template<class Address>
sese::net::AddressPool<Address> sese::net::AddressPool<Address>::pool;

template<class Address>
std::shared_ptr<Address> sese::net::AddressPool<Address>::lookup(const std::string &domain) noexcept {
    auto iterator = pool.addressMap.find(domain);
    if (iterator == pool.addressMap.end()) {
        auto address = Address::lookUpAny(domain);
        if (nullptr == address) {
            // 未命中缓存且查找失败
            return nullptr;
        } else {
            auto ipv4 = dynamicPointerCast<Address>(address);
            pool.addressMap[domain] = ipv4;
            return ipv4;
        }
    } else {
        return iterator->second;
    }
}