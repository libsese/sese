/// \author kaoru
/// \file HttpServer.h
/// \brief 简易的 Http 服务器
/// \date 2022年7月11日
/// \version 0.2

#pragma once
#include <sese/net/TcpServer.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/concurrent/ConcurrentObjectPool.h>
#include <sese/Timer.h>
#include <map>

namespace sese::http {

    using HttpRequest = RequestHeader;
    using HttpResponse = ResponseHeader;

    /// Http 服务上下文
    class API HttpServiceContext final {
    public:
        using Ptr = std::shared_ptr<HttpServiceContext>;

        /// 初始化服务上下文
        HttpServiceContext() noexcept;
        /// 重置服务上下文
        void reset(IOContext *context) noexcept;

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
        int64_t write(const void *buffer, size_t size) noexcept;

        /// 发送响应头并切换至只写模式
        bool flush() noexcept;

        [[nodiscard]] bool isReadOnly() const noexcept { return _isReadOnly; }

        // 调试用代码
        // auto getSocket() { return ioContext->getSocket(); }

    private:
        /*
         * ServiceContext 一次完整的服务包含两个部分，
         * 读取请求信息（只读模式），发送响应信息（只写模式）。
         * _isReadOnly 用于指示当前模式是否为只读模式。
         */
        bool _isReadOnly = true;
        IOContext *ioContext = nullptr;
        HttpRequest::Ptr request = nullptr;
        HttpResponse::Ptr response = nullptr;
    };

    /// Http 服务器
    class API HttpServer final {
    public:
        using Ptr = std::unique_ptr<HttpServer>;

        /// 创建一个 Http 服务器
        /// \param ipAddress 绑定的地址
        /// \param threads 线程数目
        /// \return 创建成功返回其指针，失败则为 nullptr
        static Ptr create(const IPAddress::Ptr &ipAddress, size_t threads = SERVER_DEFAULT_THREADS) noexcept;
        void loopWith(const std::function<void(const HttpServiceContext::Ptr &serviceContext)> &handler);
        void shutdown();

    private:
        struct SocketPtrCmp {
            bool operator()(const Socket::Ptr& rv, const Socket::Ptr& lv) const {
                return rv->getRawSocket() < lv->getRawSocket();
            }
        };

        using Map = std::map<Socket::Ptr, TimerTask::Ptr, SocketPtrCmp>;

        explicit HttpServer() = default;
        static void closeCallback(Map &taskMap, const Socket::Ptr& socket) noexcept;

        Map taskMap;
        std::mutex mutex;
        Timer::Ptr timer = nullptr;
        TcpServer::Ptr tcpServer = nullptr;
        concurrent::ConcurrentObjectPool<HttpServiceContext>::Ptr objectPool = nullptr;
    };
}// namespace sese::http