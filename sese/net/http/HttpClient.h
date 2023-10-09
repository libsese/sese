/// \file HttpClient.h
/// \author kaoru
/// \date 2023/07/24
/// \version 0.2
/// \brief HTTP 客户端

#pragma once

#include <sese/net/Socket.h>
#include <sese/security/SSLContext.h>
#include <sese/security/SecuritySocket.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/util/Noncopyable.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

/// HTTP 客户端
class API HttpClient final : public sese::Noncopyable {
public:
    /// 客户端智能指针对象
    using Ptr = std::unique_ptr<HttpClient>;

public:
    /** \brief 解析 URL 并创建 Http 客户端
         *  \param url URL 链接
         *  \verbatim
            https://www.example.com/index.html?val=123
            http://localhost:8080/
            http://127.0.0.1:8080/index.html
            \endverbatim
         *  \param keepAlive 是否启用长连接（需要服务器支持）
         *  \retval nullptr 创建客户端失败
         **/
    static HttpClient::Ptr create(const std::string &url, bool keepAlive = false) noexcept;

    /// 创建指定 IP 地址的 Http 客户端
    /// \param address IP 地址
    /// \param ssl 是否启用 ssl
    /// \param keepAlive 是否启用长连接（需要服务器支持）
    /// \retval nullptr 创建客户端失败
    static HttpClient::Ptr create(const IPv4Address::Ptr &address, bool ssl = false, bool keepAlive = false) noexcept;

    ~HttpClient() noexcept;

    bool doRequest() noexcept;

    Request &getRequest() { return req; }

    Response &getResponse() { return resp; }

private:
#ifdef SESE_BUILD_TEST
public:
#endif
    HttpClient() = default;

    static IPv4Address::Ptr parseAddress(const std::string &host) noexcept;

    bool reconnect() noexcept;

    Socket::Ptr socket;
    IPv4Address::Ptr address;
    // 如果是 https 协议需要 SSL 上下文
    security::SSLContext::Ptr sslContext;

    Request req;
    Response resp;

    // 默认启用长连接
    bool isKeepAlive = true;

    io::ByteBuilder buffer{4096};
};
} // namespace sese::net::http