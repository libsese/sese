#pragma once

#include <sese/service/HttpService.h>
#include <sese/net/http/Http2Connection.h>
#include <sese/net/http/Http2FrameInfo.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    struct API Http2Config : public  HttpConfig {
        std::string upgradePath = "/";
    };

    class API Http2Service : public HttpService {
    public:
        explicit Http2Service(Http2Config *config);

        ~Http2Service() noexcept override;

    protected:
        void onClose(event::BaseEvent *event) override;

        void onHandle(net::http::HttpConnection *conn) noexcept override;

        void onHandleUpgrade(net::http::HttpConnection *conn) noexcept override;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

    private:
        static void requestFromHttp2(net::http::Request &request) noexcept;

        static void responseToHttp2(net::http::Response &response) noexcept;

        static void writeFrame(net::http::HttpConnection *conn, const net::http::Http2FrameInfo &info) noexcept;

        static void writeAck(net::http::HttpConnection *conn) noexcept;

        static void writeHeader(net::http::Http2Connection *conn2, const net::http::Http2Stream::Ptr &stream) noexcept;

        static void writeData(net::http::HttpConnection *conn, const net::http::Http2Stream::Ptr &stream) noexcept;

        static bool readFrame(net::http::HttpConnection *conn, net::http::Http2FrameInfo &info) noexcept;

        static void onSettingsFrame(net::http::Http2Connection *conn2, net::http::Http2FrameInfo &info) noexcept;

        static void onWindowUpdateFrame(net::http::Http2Connection *conn2, net::http::Http2FrameInfo &info) noexcept;

        static void onHeadersFrame(net::http::Http2Connection *conn2, net::http::Http2FrameInfo &info) noexcept;

        static void onDataFrame(net::http::Http2Connection *conn2, net::http::Http2FrameInfo &info) noexcept;

        void onHandleHttp2(net::http::HttpConnection *conn) noexcept;

        std::map<net::http::HttpConnection *, net::http::Http2Connection *> conn2Map;
    };
}// namespace sese::service