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

        Server() = default;
        ~Server() noexcept = default;
        void enroll(const std::string &name, Func func) noexcept;
        void serve(const IPAddress::Ptr &address, size_t threads = SERVER_DEFAULT_THREADS) noexcept;
        void shutdown() noexcept;

    private:
        TcpServer::Ptr tcpServer;
        std::map<std::string, Func> map;
    };
}