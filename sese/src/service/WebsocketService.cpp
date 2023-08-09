#include <sese/service/WebsocketService.h>
#include <sese/net/ws/WebsocketAuthenticator.h>
#include <sese/util/Util.h>

#define CONFIG ((sese::service::WebsocketConfig *) this->config)
#define RECV_BUFFER (conn->req.getBody())
#define SEND_BUFFER (conn->resp.getBody())
#define TEMP_BUFFER (conn->buffer)

sese::service::WebsocketService::WebsocketService(sese::service::WebsocketConfig *config) noexcept
    : HttpService(config) {
}

sese::service::WebsocketService::~WebsocketService() noexcept {
}

void sese::service::WebsocketService::onHandleUpgrade(sese::net::http::HttpConnection *conn) noexcept {
    auto &req = conn->req;
    auto &resp = conn->resp;
    auto fail = [&]() {
        resp.setCode(200);
        if (conn->timeoutEvent) {
            this->freeTimeoutEvent(conn->timeoutEvent);
            conn->timeoutEvent = nullptr;
        }
    };

    if (req.getType() != net::http::RequestType::Get) {
        fail();
        return;
    }

    if (req.getUrl() != CONFIG->upgradePath) {
        fail();
        return;
    }

    auto upgrade = req.get("upgrade", "undef");
    if (!sese::strcmpDoNotCase(upgrade.c_str(), "websocket")) {
        fail();
        return;
    }

    auto websocketKey = req.get("sec-websocket-key", "");
    auto websocketVersion = req.get("sec-websocket-version", "");
    if (websocketKey.empty() || websocketVersion != "13") {
        fail();
        return;
    }

    auto accept = net::ws::WebsocketAuthenticator::toResult(websocketKey.c_str());
    resp.setCode(101);
    resp.set("sec-websocket-accept", accept.get());
    resp.set("sec-websocket-version", "13");
    resp.set("connection", "upgrade");
    resp.set("upgrade", "websocket");
    sessionMap[conn] = new net::ws::WebsocketSession;
}

void sese::service::WebsocketService::onHandleWebsocket(net::http::HttpConnection *conn) noexcept {

}

void sese::service::WebsocketService::onHandle(sese::net::http::HttpConnection *conn) noexcept {
    auto iterator = sessionMap.find(conn);
    /// 标准 Http 处理
    if (iterator == sessionMap.end()) {
        HttpService::onHandle(conn);
        return;
    }

    /// Websocket 处理
    onHandleWebsocket(conn);
}

void sese::service::WebsocketService::onWrite(sese::event::BaseEvent *event) {
    auto conn = (sese::net::http::HttpConnection *) event->data;
    auto iterator = sessionMap.find(conn);
    /// 标准 Http 处理
    if (iterator == sessionMap.end()) {
        HttpService::onWrite(event);
        return;
    }

    /// Websocket 处理
    onWebSocketWrite(event, iterator->second);
}

void sese::service::WebsocketService::onWebSocketWrite(event::BaseEvent *event, net::ws::WebsocketSession *session) noexcept {

}

void sese::service::WebsocketService::onClose(sese::event::BaseEvent *event) {
    auto conn = (sese::net::http::HttpConnection *) event->data;
    auto iterator = sessionMap.find(conn);
    if (iterator != sessionMap.end()) {
        delete iterator->second;
        sessionMap.erase(iterator);
    }
    HttpService::onClose(event);
}

void sese::service::WebsocketService::onTimeout(sese::service::TimeoutEvent *timeoutEvent) {
    auto conn = (sese::net::http::HttpConnection *) timeoutEvent->data;
    auto iterator = sessionMap.find(conn);
    if (iterator != sessionMap.end()) {
        delete iterator->second;
        sessionMap.erase(iterator);
    }
    HttpService::onTimeout(timeoutEvent);
}
