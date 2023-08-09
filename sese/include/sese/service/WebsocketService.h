#pragma once

#include <sese/service/HttpService.h>
#include <sese/net/ws/WebsocketSession.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    struct API WebsocketConfig : public HttpConfig {
        std::string upgradePath = "/";
    };

    class API WebsocketService : public HttpService {
    public:
        explicit WebsocketService(WebsocketConfig *config) noexcept;

        ~WebsocketService() noexcept override;

    protected:
        void onHandle(net::http::HttpConnection *conn) noexcept override;
        void onHandleUpgrade(net::http::HttpConnection *conn) noexcept override;

        void onWrite(event::BaseEvent *event) override;
        void onClose(event::BaseEvent *event) override;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

    private:
        /// 解析 websocket 帧并构建消息
        /// \param conn Http 连接
        void onHandleWebsocket(net::http::HttpConnection *conn) noexcept;
        /// 发送 websocket 组装完成的帧缓存
        /// \param event
        /// \param session
        void onWebSocketWrite(event::BaseEvent *event, net::ws::WebsocketSession *session) noexcept;

        std::map<net::http::HttpConnection *, net::ws::WebsocketSession *> sessionMap;
    };
}// namespace sese::service