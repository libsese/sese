/// \file HttpClient.h
/// \author kaoru
/// \date 2023/07/24
/// \version 0.2
/// \brief Http 客户端

#pragma once

#include <sese/net/Socket.h>
#include <sese/security/SSLContext.h>
#include <sese/security/SecuritySocket.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/util/Noncopyable.h>

namespace sese::net::http {

    /// Http 客户端
    class API HttpClient final : public sese::Noncopyable {
    public:
        using Ptr = std::unique_ptr<HttpClient>;

    public:
        /// \brief 解析 URL 并创建 Http 客户端
        /// \param url URL 链接
        /// \param keepAlive 是否启用长连接（需要服务器支持）
        /// \retval nullptr 创建客户端失败
        /// \example https://www.example.com/index.html?val=123
        /// \example http://localhost:8080/
        /// \example http://127.0.0.1:8080/index.html
        static HttpClient::Ptr create(const std::string &url, bool keepAlive = false) noexcept;

        ~HttpClient() noexcept;

        /// \brief 从 Http 流中读取字节
        /// \param buffer 缓存
        /// \param len 缓存大小
        /// \return 实际读取到的字节
        int64_t read(void *buffer, size_t len) noexcept;

        /// \brief 向 Http 流中写入字节
        /// \param buffer 缓存
        /// \param len 缓存大小
        /// \return 实际写入的字节
        int64_t write(const void *buffer, size_t len) noexcept;

        bool doRequest() noexcept;

        bool doResponse() noexcept;

        RequestHeader &getRequest() { return req; }

        ResponseHeader &getResponse() { return resp; }

        /// \brief 获取响应正文长度
        /// \return 响应正文长度
        [[nodiscard]] int64_t getResponseContentLength() const noexcept { return responseContentLength; }

    private:
        HttpClient() = default;

        static IPv4Address::Ptr parseAddress(const std::string &host) noexcept;

        bool reconnect() noexcept;

        Socket::Ptr socket;
        IPv4Address::Ptr address;
        // 如果是 https 协议需要 SSL 上下文
        security::SSLContext::Ptr sslContext;

        RequestHeader req;
        ResponseHeader resp;

        // 该字段在获取响应后可用
        int64_t responseContentLength = 0;
        // 默认启用长连接
        bool isKeepAlive = true;
    };
}// namespace sese::net::http