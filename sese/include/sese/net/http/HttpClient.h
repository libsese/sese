/// \file HttpClient.h
/// \author kaoru
/// \date 2022/07/29
/// \version 0.1
/// \brief Http 客户端
#pragma once
#include <sese/net/Socket.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include "sese/util/Noncopyable.h"

namespace sese::net::http {

    /// Http 客户端
    class API HttpClient final : public sese::Noncopyable {
    public:
        using Ptr = std::unique_ptr<HttpClient>;

        /// \brief 发起 Http 请求连接
        /// \param domain 连接域名
        /// \param port 连接端口
        /// \retval 连接成功返回客户端对象
        /// \retval nullptr 创建失败
        static HttpClient::Ptr create(const std::string &domain, uint16_t port = 80) noexcept;

    public:
        /// \brief 释放 Http 连接
        ~HttpClient() noexcept;

        /// \brief 发送一次请求
        /// \param request 请求头
        /// \retval true 发送成功
        /// \retval false 发送失败
        bool request(RequestHeader *request) noexcept;

        /// \brief 获取一次响应
        /// \retval 获取成功返回响应头
        /// \retval nullptr 获取失败
        ResponseHeader::Ptr response() noexcept;

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

        /// \brief 检查当前连接的可用性（在复用连接前用其判断连接是否存在）
        /// \return 是否可用
        bool check() noexcept;

        /// \brief 释放资源
        void dispose() noexcept;

        /// \brief 分离 socket
        /// \return socket
        socket_t detach() noexcept;

        /// \brief 获取响应正文长度
        /// \return 响应正文长度
        [[nodiscard]] int64_t getResponseContentLength() const noexcept { return responseContentLength; }

    private:
        /// \brief 私有构造函数
        HttpClient() = default;
        Socket::Ptr clientSocket;
        // 该字段在获取响应后可用
        int64_t responseContentLength = 0;
        // 默认启用长连接
        bool isKeepAlive = true;
        bool isDetach = false;
    };
}// namespace sese::http