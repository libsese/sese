/// \file WebsocketService.h
/// \brief Websocket 服务
/// \date 2023年8月10日
/// \author kaoru
/// \version 0.1

#pragma once

#include <sese/service/HttpService.h>
#include <sese/net/ws/FrameHeader.h>
#include <sese/net/ws/WebsocketEvent.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    /// Websocket 配置
    struct API WebsocketConfig : public HttpConfig {
        std::string upgradePath = "/";
    };

    /// Websocket 服务
    class API WebsocketService : public HttpService {
    public:
        /// 实例化 Websocket 服务
        /// \param config 配置
        /// \param event 响应事件
        explicit WebsocketService(WebsocketConfig *config, net::ws::WebsocketEvent *event) noexcept;

        ~WebsocketService() noexcept override;

        /// 发送消息
        /// \param session Websocket 会话
        /// \note 此函数只能在 Event 回调函数中调用，如果需要主动推送数据，可以考虑设置定时事件或者继承服务自定义
        void doWriteMessage(net::ws::WebsocketSession *session) noexcept;

        /// 发送二进制数据
        /// \param session Websocket 会话
        /// \note 此函数只能在 Event 回调函数中调用，如果需要主动推送数据，可以考虑设置定时事件或者继承服务自定义
        void doWriteBinary(net::ws::WebsocketSession *session) noexcept;

    protected:
        void onHandle(net::http::HttpConnection *conn) noexcept override;

        void onHandleUpgrade(net::http::HttpConnection *conn) noexcept override;

        void onClose(event::BaseEvent *event) override;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

    private:
        /// 响应 pong 帧
        /// \param conn 连接
        /// \param info 帧信息
        /// \param session Websocket 会话
        void doPong(net::http::HttpConnection *conn, net::ws::FrameHeaderInfo &info, net::ws::WebsocketSession *session) noexcept;

        /// 响应 close 帧
        /// \param conn 连接
        /// \param session Websocket 会话
        void doClose(net::http::HttpConnection *conn, net::ws::WebsocketSession *session) noexcept;

        /// 解析 websocket 帧并构建消息
        /// \param conn Http 连接
        void onHandleWebsocket(net::http::HttpConnection *conn) noexcept;

        std::map<net::http::HttpConnection *, net::ws::WebsocketSession *> sessionMap;
        net::ws::WebsocketEvent *event = nullptr;
    };
}// namespace sese::service