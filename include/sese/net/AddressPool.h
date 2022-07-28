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

namespace sese {

    /// IPv4 地址池
    class API IPv4AddressPool final {
    public:
        /// 获取域名与 IP 地址的映射关系
        /// \param domain 域名
        /// \retval nullptr 未命中缓存且查找失败
        /// \retval other 查找到地址
        static IPv4Address::Ptr lookup(const std::string &domain) noexcept;

    private:
        IPv4AddressPool() = default;

        static IPv4AddressPool pool;

        std::map<std::string, IPv4Address::Ptr> addressMap;
    };
}// namespace sese