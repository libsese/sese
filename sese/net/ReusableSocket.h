/// \file ReusableSocket.h
/// \author kaoru
/// \brief 可重用地址和端口的套接字构建器
/// \date 2023年6月2日
/// \version 0.1

#pragma once

#include <sese/net/Socket.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SecuritySocket.h>

#include <optional>

namespace sese::net {

    /// \brief 可重用地址和端口的套接字构建器
    /// \warning 用此构建器构建出来的 Socket 在不同平台上行为不一致 <p>
    /// Linux: 内核会负责负载均衡，多个 Socket 都能接收到连接 <p>
    /// Darwin: 无负载均衡，只有最后一个 Socket 能接收到连接 <p>
    /// Windows: 无负载均衡，只有第一个 Socket 能接收到连接
    /// \see https://www.cnblogs.com/xybaby/p/7341579.html
    class ReusableSocket {
    public:
        /// 初始化可重用地址和端口的套接字模板
        /// \param address 地址
        /// \param type 类型
        explicit ReusableSocket(IPAddress::Ptr address, Socket::Type type = Socket::Type::TCP);

        /// 按照模板构建一个 native socket
        /// \retval -1 创建失败
        /// \return native socket
        socket_t makeRawSocket() noexcept;

        /// 按照模板构建一个 sese::net::Socket
        /// \retval nullptr 创建失败
        /// \return sese::net::Socket
        std::optional<Socket> makeSocket() noexcept;

    protected:
        std::optional<Socket> builtinMakeSocket() noexcept;

    protected:
        IPAddress::Ptr addr{};
        Socket::Type type{};
    };

}