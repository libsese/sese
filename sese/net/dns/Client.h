/// \file Client.h
/// \author kaoru
/// \brief DNS 客户端类
/// \version 0.1
/// \date 2023年8月13日
/// \see sese::net::dns::DNSServer

#pragma once

#include <sese/net/IPv6Address.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::dns {

    /// DNS 客户端
    class API Client {
    public:
        /// 使用系统的域名解析器
        /// \param domain 域名
        /// \param family 协议族
        /// \param type 类型
        /// \param protocol 协议
        /// \return IP 地址
        /// \retval nullptr 查找失败
        [[maybe_unused]] static Address::Ptr resolveSystem(
                const std::string &domain,
                int family = AF_INET,
                int type = 0,
                int protocol = 0
        ) noexcept;

        /// 使用其他域名解析器
        /// \param domain 域名
        /// \param server DNS 服务器地址
        /// \param family 协议族
        /// \param type 类型
        /// \param protocol 协议
        /// \return IP 地址
        /// \retval nullptr 查找失败
        static sese::net::Address::Ptr resolveCustom(
                const std::string &domain,
                const IPAddress::Ptr &server,
                int family = AF_INET,
                int type = 0,
                int protocol = 0
        ) noexcept;

        /// 优先使用其他域名解析器、其次使用系统域名解析器
        /// \param domain 域名
        /// \param server DNS 服务器地址
        /// \param family 协议族
        /// \param type 类型
        /// \param protocol 协议
        /// \return IP 地址
        /// \retval nullptr 查找失败
        static Address::Ptr resolveAuto(
                const std::string &domain,
                const IPAddress::Ptr& server,
                int family = AF_INET,
                int type = 0,
                int protocol = 0
        ) noexcept;
    };
}// namespace sese::net::dns