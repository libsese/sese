/// \author kaoru
/// \file HttpServer.h
/// \brief 简易的 Http 服务器
/// \date 2022年7月10日
/// \version 0.1
/// \warning 未实现

#pragma once
#include <sese/net/TcpServer.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>

namespace sese::http {

    /// Http 服务上下文
    class HttpServiceContext {
    public:
        using Ptr = std::shared_ptr<HttpServiceContext>;
        using HttpRequest = RequestHeader;
        using HttpResponse = ResponseHeader;

        /// 初始化服务上下文
        HttpServiceContext(IOContext *ioContext) noexcept;
        /// 重置服务上下文
        void reset(IOContext *ioContext) noexcept;

        /// 获取请求头
        /// \return 请求头信息
        [[nodiscard]] const HttpRequest::Ptr &getRequest() const noexcept { return request; }
        /// 获取响应头信息
        /// \note 此举会将服务模式变更为只写模式
        /// \return 响应头信息
        [[nodiscard]] const HttpResponse::Ptr &getResponse() const noexcept { return response; }

        /// 读取请求的正文信息
        /// \param buffer 缓存
        /// \param size 长度
        /// \return 读取到的长度
        int64_t read(void *buffer, size_t size) noexcept;
        /// 立即向服务对象发送响应头，并发送正文信息
        /// \param buffer 正文信息缓存
        /// \param size 长度
        /// \return 已发送的长度
        int64_t write(void *buffer, size_t size) noexcept;

    private:
        /*
         * ServiceContext 一次完整的服务包含两个部分，
         * 读取请求信息（只读模式），发送响应信息（只写模式）。
         * isReadOnly 用于指示当前模式是否为只读模式。
         */
        bool isReadOnly = true;
        IOContext *ioContext = nullptr;
        HttpRequest::Ptr request = nullptr;
        HttpResponse::Ptr response = nullptr;
    };

    /// Http 服务器
    class HttpServer : private sese::TcpServer {
    public:
        using Ptr = std::unique_ptr<HttpServer>;
        Ptr create(const IPAddress::Ptr &ipAddress, size_t threads) noexcept;
        void loopWith(std::function<void (HttpServiceContext &serviceContext)> proc);
        void shutdown();

    private:
        HttpServer();
    };
}// namespace sese::http