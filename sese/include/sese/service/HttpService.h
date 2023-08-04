#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/Config.h>
#include <sese/security/SSLContext.h>
#include <sese/net/http/ControllerGroup.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    struct API HttpConfig {
        HttpConfig() noexcept;

        void setController(const std::string &path, const net::http::SimpleController &controller1) noexcept;

        void setController(const std::string &path, const net::http::Controller &controller2) noexcept;

        void setController(const net::http::ControllerGroup &group) noexcept;

        std::string servName = HTTPD_NAME;          // 响应中自动添加 Server 字段值
        security::SSLContext::Ptr servCtx = nullptr;// SSL 上下文，为空则不开启 HTTPS
        uint64_t keepalive = 0;                     // 连接保留时间，为 0 则每次响应都会关闭连接
        std::string workDir;                        // 服务器工作目录，如果为空则不开启文件下载功能

        net::http::Controller otherController;
        std::map<std::string, net::http::Controller> controllerMap;
    };

    // class API HttpService final : public sese::event::EventLoop {
    class API HttpService final : public sese::service::TimerableService {
    public:
        explicit HttpService(const HttpConfig &config) noexcept;

        ~HttpService() noexcept override;

    private:
        void onAccept(int fd) override;

        void onRead(event::BaseEvent *event) override;

        void onWrite(event::BaseEvent *event) override;

        void onClose(event::BaseEvent *event) override;

    private:
        event::BaseEvent *createEventEx(int fd, unsigned int events, void *data) noexcept;

        void freeEventEx(event::BaseEvent *event) noexcept;

    private:
        void onHandle(net::http::HttpConnection *conn) noexcept;

        void onHandleFile(net::http::HttpConnection *conn, const std::string &path) noexcept;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

        void onControllerWrite(event::BaseEvent *event) noexcept;

        void onFileWrite(event::BaseEvent *event) noexcept;

        static std::map<std::string, std::string> contentTypeMap;

    private:
        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        HttpConfig config;
        std::map<int, event::BaseEvent *> eventMap;
    };
}// namespace sese::service