#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/Config.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/net/http/Range.h>
#include <sese/security/SSLContext.h>
#include <sese/service/TimerableService.h>
#include <sese/util/ByteBuilder.h>
#include <sese/util/FileStream.h>

#include <map>
#include <functional>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    /// 指示 Http 处理状态
    enum class HttpHandleStatus {
        HANDING,/// 指示当前请求即将进入处理状态
        FAIL,   /// 指示当前请求处理失败，已无法返回任何报文
        OK,     /// 指示当前请求处理完成，需要返回报文
        FILE    /// 指示当前请求是一个下载文件的请求，需要在 onWrite 时读取文件
    };

    struct API HttpConnection final : public sese::Stream {
        int fd = 0;
        void *ssl = nullptr;

        HttpHandleStatus status = HttpHandleStatus::HANDING;
        net::http::RequestHeader req;
        net::http::ResponseHeader resp;

        FileStream::Ptr file;
        size_t fileSize = 0;

        ByteBuilder buffer1{4096};// 请求缓冲区
        ByteBuilder buffer2{4096};// 响应缓冲区
        uint64_t requestSize = 0; // 请求大小

        std::string contentType = "application/x-";           // 默认响应内容类型 - 仅在 status 为 FILE 且 存在多段 Range 时使用
        std::vector<net::http::Range> ranges;                 // 响应区间集合 - 仅在 status 为 FILE 时使用
        std::vector<net::http::Range>::iterator rangeIterator;// 指示当前响应区间迭代器

        event::BaseEvent *event = nullptr;
        TimeoutEvent *timeoutEvent = nullptr;

        ~HttpConnection() noexcept;

        void doResponse() noexcept;

        int64_t read(void *buf, size_t len) override;

        int64_t write(const void *buf, size_t len) override;
    };

    struct API HttpConfig {
        using Controller1 = std::function<
                bool(
                        net::http::RequestHeader &req,
                        net::http::ResponseHeader &resp
                )>;
        using Controller2 = std::function<
                void(
                        HttpConnection *conn
                )>;

    public:
        HttpConfig() noexcept;

        void setController(const std::string &path, const Controller1 &controller1) {
            this->controller1Map[path] = controller1;
        }

        void setController(const std::string &path, const Controller2 &controller2) {
            this->controller2Map[path] = controller2;
        }

        std::string servName = HTTPD_NAME;          // 响应中自动添加 Server 字段值
        security::SSLContext::Ptr servCtx = nullptr;// SSL 上下文，为空则不开启 HTTPS
        uint64_t keepalive = 0;                     // 连接保留时间，为 0 则每次响应都会关闭连接
        std::string workDir;                        // 服务器工作目录，如果为空则不开启文件下载功能

        Controller2 otherController;
        std::map<std::string, Controller1> controller1Map;
        std::map<std::string, Controller2> controller2Map;
    };

    // class API HttpService final : public sese::event::EventLoop {
    class API HttpService final : public sese::service::TimerableService {
    public:
        explicit HttpService(const HttpConfig &config) noexcept;

    private:
        void onAccept(int fd) override;

        void onRead(event::BaseEvent *event) override;

        void onWrite(event::BaseEvent *event) override;

        void onClose(event::BaseEvent *event) override;

    private:
        void onHandle(HttpConnection *conn) noexcept;

        void onHandleFile(sese::service::HttpConnection *conn, const std::string &path) noexcept;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

        void onControllerWrite(event::BaseEvent *event) noexcept;

        void onFileWrite(event::BaseEvent *event) noexcept;

        static std::map<std::string, std::string> contentTypeMap;

    private:
        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        HttpConfig config;
    };
}// namespace sese::service