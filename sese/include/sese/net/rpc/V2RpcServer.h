/// \file V2RpcServer.h
/// \author kaoru
/// \brief Rpc 服务端选项
/// \version 0.2
/// \date 2023.04.29

#pragma once

//#ifdef _WIN32
//#pragma warning(disable: 4996)
//#elif __clang__
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdeprecated-declarations"
//#elif __GNUC__
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//#endif

#include "sese/net/V2Server.h"
#include "sese/config/json/JsonTypes.h"

namespace sese::net::v2::rpc {

    /// Rpc 服务选项
    class API SESE_DEPRECATED V2RpcServer : public Server {
    public:
        /// 服务子程序
        /// 参数与返回值均为 sese::json::ObjectData::Ptr
        using Func = std::function<void(json::ObjectData::Ptr &, json::ObjectData::Ptr &)>;

        void onHandle(sese::net::v2::IOContext &ctx) noexcept override;

        void setFunction(const std::string &name, const Func &func) noexcept;
    protected:
        /// 此选项用于注册服务
        std::map<std::string, Func> map;
    };
}// namespace sese::net::v2::rpc

//#ifdef __clang__
//#pragma clang diagnostic pop
//#elif __GNUC__
//#pragma GCC diagnostic pop
//#endif