#pragma once

#include <sese/Config.h>
#include <sese/net/ws/WebsocketSession.h>

namespace sese::service {
    class API WebsocketService;
}

namespace sese::net::ws {
    class API WebsocketEvent {
    public:
        virtual void onMessage(service::WebsocketService* service, WebsocketSession *session) noexcept = 0;
        virtual void onBinary(service::WebsocketService* service, WebsocketSession *session) noexcept = 0;
        virtual void onClose(service::WebsocketService* service, WebsocketSession *session) noexcept = 0;
        virtual void onTimeout(service::WebsocketService* service, WebsocketSession *session) noexcept = 0;
    };
}// namespace sese::net::ws