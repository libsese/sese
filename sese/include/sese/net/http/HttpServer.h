/// \author kaoru
/// \file HttpServer.h
/// \brief 简易的 Http 服务器
/// \date 2022年7月11日
/// \version 0.2
/// \warning 实验性功能，仅在 Linux 下启用长连接

#pragma once
#include <sese/security/SecurityTcpServer.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/net/http/HttpUtil.h>
// #include <sese/util/Timer.h>
// #include <map>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

namespace sese::net::http {

    using HttpRequest = RequestHeader;
    using HttpResponse = ResponseHeader;

    /// Http 服务上下文
    template<typename Context>
    class API HttpServiceContext final : public sese::Stream {
    public:
        using Ptr = std::shared_ptr<HttpServiceContext>;

        HttpRequest request;
        HttpResponse response;

        /// 重置服务上下文
        void reset(Context *context) noexcept {
            ioContext = context;
        }

        /// 读取请求的正文信息
        /// \param buffer 缓存
        /// \param size 长度
        /// \return 读取到的长度
        int64_t read(void *buffer, size_t size) noexcept override {
            return ioContext->read(buffer, size);
        }

        /// 立即向服务对象发送响应头，并发送正文信息
        /// \param buffer 正文信息缓存
        /// \param size 长度
        /// \return 已发送的长度
        int64_t write(const void *buffer, size_t size) noexcept override {
            if (!_isFlushed) {
                // try to flush resp
                if (!flush()) {
                    return -1;
                }
            }
            return ioContext->write(buffer, size);
        }

        /// 发送响应头并切换至只写模式
        bool flush() noexcept {
            response.set("Server", HTTPD_NAME);
            if (0 == strcasecmp(request.get("Connection", "Keep-Alive").c_str(), "Close")) {
                response.set("Connection", "Close");
            } else {
                response.set("Connection", "Keep-Alive");
            }

            _isFlushed = true;
            if (!HttpUtil::sendResponse(this, &response)) {
                _isFlushed = false;
                return false;
            } else {
                return true;
            }
        }

        void close() override {
            ioContext->close();
        }

        [[nodiscard]] bool isFlushed() const noexcept { return _isFlushed; }

        socket_t getRawSocket() { return ioContext->socket; }

    private:
        bool _isFlushed = false;
        Context *ioContext = nullptr;
    };

    /// Http 服务器
    //    class API HttpServer final {
    //    public:
    //        using Ptr = std::unique_ptr<HttpServer>;
    //        using ServiceContext = HttpServiceContext<IOContext>;
    //        using PreHandler = std::function<bool(IOContext *)>;
    //        using Handler = std::function<void(ServiceContext *)>;
    //
    //        /// 创建一个 Http 服务器
    //        /// \param ipAddress 绑定的地址
    //        /// \param threads 线程数目
    //        /// \param keepAlive Keep-Alive 时长
    //        /// \return 创建成功返回其指针，失败则为 nullptr
    //        static HttpServer::Ptr create(const IPAddress::Ptr &ipAddress, size_t threads = SERVER_DEFAULT_THREADS, size_t keepAlive = SERVER_KEEP_ALIVE_DURATION) noexcept;
    //        void loopWith(const Handler &handler);
    //        void shutdown();
    //
    //    private:
    //        explicit HttpServer() = default;
    //
    //        TcpServer::Ptr tcpServer = nullptr;
    //    };
    //
    //    class API HttpsServer {
    //    public:
    //        using Ptr = std::unique_ptr<HttpsServer>;
    //        using ServiceContext = HttpServiceContext<security::IOContext>;
    //        using Handler = std::function<void(ServiceContext *)>;
    //
    //        struct SecurityOptions {
    //            const char *CA = nullptr;
    //            const char *KEY = nullptr;
    //        };
    //
    //        static HttpsServer::Ptr create(
    //                const IPAddress::Ptr &ipAddress,
    //                size_t threads,
    //                size_t keepAlive,
    //                SecurityOptions options
    //        ) noexcept;
    //        void loopWith(const Handler &handler);
    //        void shutdown();
    //
    //    private:
    //        explicit HttpsServer() = default;
    //        security::SecurityTcpServer::Ptr tcpServer = nullptr;
    //    };
}// namespace sese::http