#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/Config.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/security/SSLContext.h>
#include <sese/service/TimerableService.h>
#include <sese/util/ByteBuilder.h>

#include <map>
#include <functional>

namespace sese::service {

    struct HttpConfig {
        using Controller1 = std::function<
                bool(
                        net::http::RequestHeader &req,
                        net::http::ResponseHeader &resp
                )>;
        using Controller2 = std::function<
                bool(
                        net::http::RequestHeader &req,
                        net::http::ResponseHeader &resp,
                        ByteBuilder *buffer
                )>;

        std::string servName = HTTPD_NAME;
        security::SSLContext::Ptr servCtx = nullptr;
        uint64_t keepalive = 0;

        std::map<std::string, Controller1> controller1Map;
        std::map<std::string, Controller2> controller2Map;
    };

    // class API HttpService final : public sese::service::TimerableService {
    class API HttpService final : public sese::event::EventLoop {
    public:
        explicit HttpService(const HttpConfig &config) noexcept;

        void onAccept(int fd) override;

        void onRead(event::BaseEvent *event) override;

        void onWrite(event::BaseEvent *event) override;

    private:
        bool onHandle(ByteBuilder *builder) noexcept;

        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        HttpConfig config;

        std::map<int, ByteBuilder *> buffers;
    };
}// namespace sese::service