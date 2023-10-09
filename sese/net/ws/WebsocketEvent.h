/**
* @file WebsocketEvent.h
* @author kaoru
* @version 0.1
* @brief Websocket 事件类
* @date 2023年9月13日
*/

#pragma once

#include <sese/Config.h>
#include <sese/net/ws/WebsocketSession.h>

namespace sese::service {
class API WebsocketService;
}

namespace sese::net::ws {

/// Websocket 事件
class API WebsocketEvent {
public:
    virtual void onMessage(service::WebsocketService *service, WebsocketSession *session) noexcept = 0;
    virtual void onBinary(service::WebsocketService *service, WebsocketSession *session) noexcept = 0;
    virtual void onClose(service::WebsocketService *service, WebsocketSession *session) noexcept = 0;
    virtual void onTimeout(service::WebsocketService *service, WebsocketSession *session) noexcept = 0;
};
} // namespace sese::net::ws