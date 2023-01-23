/**
 * \file Server.h
 * \author kaoru
 * \brief RPC 服务端
 * \version 0.1
 */
#pragma once
#include <sese/net/rpc/Client.h>
#include <map>
#include <functional>

namespace sese::rpc {

    /// RPC 服务端
    class API Server final {
    public:
        using Ptr = std::unique_ptr<Server>;
        using Func = std::function<void (json::ObjectData::Ptr &, json::ObjectData::Ptr &)>;

        ~Server() noexcept = default;
        /// 创建 RPC 服务器
        /// \param address 绑定地址
        /// \param threads 线程池大小
        /// \retval nullptr tcp server 创建失败
        static Server::Ptr create(const IPAddress::Ptr &address, size_t threads = SERVER_DEFAULT_THREADS) noexcept;
        /// 向服务器注册远程调用
        /// \param name 调用名称
        /// \param func 本地方法
        void enroll(const std::string &name, Func func) noexcept;
        /// 开始监听服务
        void serve() noexcept;
        /// 阻塞并等待服务结束
        void shutdown() noexcept;

    private:
        Server() noexcept = default;

        TcpServer::Ptr tcpServer;
        std::map<std::string, Func> map;
    };
}