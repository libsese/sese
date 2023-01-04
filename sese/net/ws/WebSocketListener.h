#pragma once

#include "sese/net/ReadableServer.h"
#include "sese/net/ws/WebSocketContext.h"

namespace sese::net::ws {

    struct API WebSocketHandler {
        using Subhandler = std::function<void(WebSocketContext *ctx)>;

        Subhandler onRead;
        Subhandler onClosed;
        Subhandler onPing;
        Subhandler onError;
    };

    class API WebSocketListener {
    public:
        using Ptr = std::unique_ptr<WebSocketListener>;

        static WebSocketListener::Ptr create(size_t threads, WebSocketHandler &handler) noexcept;
        void shutdown() noexcept;

        void distribute(sese::net::IOContext *context) const noexcept;

    private:
        WebSocketListener() = default;
        ReadableServer::Ptr server;
        WebSocketHandler handler;
    };
}// namespace sese::net::ws