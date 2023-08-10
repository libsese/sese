#pragma once

#include <sese/service/HttpService.h>
#include <sese/net/ws/FrameHeader.h>
#include <sese/net/ws/WebsocketEvent.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    struct API WebsocketConfig : public HttpConfig {
        std::string upgradePath = "/";
    };

    class API WebsocketService : public HttpService {
    public:
        explicit WebsocketService(WebsocketConfig *config, net::ws::WebsocketEvent *event) noexcept;

        ~WebsocketService() noexcept override;

        void doWriteMessage(net::ws::WebsocketSession *session) noexcept;

        void doWriteBinary(net::ws::WebsocketSession *session) noexcept;

    protected:
        void onHandle(net::http::HttpConnection *conn) noexcept override;

        void onHandleUpgrade(net::http::HttpConnection *conn) noexcept override;

        void onClose(event::BaseEvent *event) override;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

    private:
        void doPong(net::http::HttpConnection *conn, net::ws::FrameHeaderInfo &info, net::ws::WebsocketSession *session) noexcept;

        void doClose(net::http::HttpConnection *conn, net::ws::WebsocketSession *session) noexcept;

        /// 解析 websocket 帧并构建消息
        /// \param conn Http 连接
        void onHandleWebsocket(net::http::HttpConnection *conn) noexcept;

        std::map<net::http::HttpConnection *, net::ws::WebsocketSession *> sessionMap;
        net::ws::WebsocketEvent *event = nullptr;
    };
}// namespace sese::service