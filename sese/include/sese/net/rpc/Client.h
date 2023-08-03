/**
 * \file Client.h
 * \brief RPC 客户端
 * \author kaoru
 * \version 0.1
 */
#pragma once

#include <sese/net/IPv6Address.h>
#include <sese/net/rpc/Marco.h>
#include <sese/config/json/JsonTypes.h>
#include <sese/security/SecuritySocket.h>
#include <sese/security/SSLContext.h>
#include <sese/util/Noncopyable.h>
#include <sese/util/ByteBuilder.h>

namespace sese::net::rpc {

    /// RPC 客户端
    class API Client final : public Noncopyable {
    public:
        using Ptr = std::unique_ptr<Client>;

        explicit Client(const IPv4Address::Ptr &address, bool ssl = false, const std::string &version = SESE_RPC_VERSION_0_1) noexcept;

        ~Client() noexcept;

        bool doRequest(const std::string &name, json::ObjectData::Ptr &args) noexcept;

        json::ObjectData::Ptr doResponse() noexcept;

    private:
        json::ObjectData::Ptr makeTemplateRequest(const std::string &name);

        bool reconnect() noexcept;

    private:
        Socket::Ptr socket;
        IPv4Address::Ptr address;
        // 如果启用 ssl 则需要 SSL 上下文
        security::SSLContext::Ptr sslContext;

        ByteBuilder buffer{};
        json::BasicData::Ptr version;
    };

    /// 获取返回码对应的错误信息
    /// \param code 返回码
    /// \return 错误信息
    API const char *getErrorMessage(int64_t code) noexcept;
}// namespace sese::net::rpc