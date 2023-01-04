#include "sese/net/ws/WebSocketListener.h"
#include "sese/util/Packaged2Stream.h"

sese::net::ws::WebSocketListener::Ptr sese::net::ws::WebSocketListener::create(size_t threads, WebSocketHandler &handler) noexcept {
    auto result = std::unique_ptr<WebSocketListener>(new WebSocketListener);
    auto server = sese::net::ReadableServer::create([&result](sese::net::IOContext *ctx){
        result->distribute(ctx);
    }, threads);
    if(server == nullptr) {
        return nullptr;
    } else {
        result->server = std::move(server);
        result->handler = handler;
        return result;
    }
}

void sese::net::ws::WebSocketListener::shutdown() noexcept {
    server->shutdown();
}

void sese::net::ws::WebSocketListener::distribute(sese::net::IOContext *context) const noexcept {
    auto stream = PackagedStream<sese::net::IOContext>(context);
    auto ws = WebSocketContext(&stream);

    if (!ws.readInfo()) {
        ws.close();
        return;
    }

    decltype(auto) info = ws.getCurrentFrameHeaderInfo();
    switch (info.opCode) {
        case SESE_WS_OPCODE_TEXT:
        case SESE_WS_OPCODE_BIN:
            if (handler.onRead) {
                handler.onRead(&ws);
            }
            break;
        case SESE_WS_OPCODE_PING:
            if (handler.onPing) {
                handler.onPing(&ws);
            }
            break;
        case SESE_WS_OPCODE_CLOSE:
            if (handler.onClosed) {
                handler.onClosed(&ws);
            }
            break;
        default:
            ws.setError(SESE_WS_ERROR_UNREGISTERED_FRAME);
            if (handler.onError) {
                handler.onError(&ws);
            }
            break;
    }

    auto err = ws.getError();
    if (handler.onError && err) {
        handler.onError(&ws);
    }
}