#include <sese/service/WebsocketService.h>
#include <sese/net/ws/WebsocketAuthenticator.h>
#include <sese/util/Endian.h>
#include <sese/util/Util.h>

#define CONFIG ((sese::service::WebsocketConfig *) this->config)
#define RECV_BUFFER (conn->req.getBody())
#define SEND_BUFFER (conn->buffer)
#define SESSION_CONN ((sese::net::http::HttpConnection *) session->data)

sese::service::WebsocketService::WebsocketService(sese::service::WebsocketConfig *config, sese::net::ws::WebsocketEvent *event) noexcept
    : HttpService(config) {
    this->event = event;
}

sese::service::WebsocketService::~WebsocketService() noexcept {
    for (decltype(auto) item : sessionMap) {
        delete item.second;
    }
    sessionMap.clear();
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
    auto session = new net::ws::WebsocketSession;
    session->fd = -conn->fd;
    session->data = conn;
    sessionMap[conn] = session;
}

#define ASSERT_READ(buf, size)                            \
    if (RECV_BUFFER.read(buf, size) != size) {            \
        conn->status = net::http::HttpHandleStatus::FAIL; \
        return;                                           \
    }                                                     \
    SESE_MARCO_END

void sese::service::WebsocketService::onHandleWebsocket(net::http::HttpConnection *conn) noexcept {
    auto session = sessionMap[conn];
    session->buffer.freeCapacity();
    size_t total = 0;
    uint8_t buffer;
    struct sese::net::ws::FrameHeaderInfo info {};
    while (!info.fin) {
        // buffer to info
        ASSERT_READ(&buffer, sizeof(buffer));
        info.fin = (buffer & 0b1000'0000) >> 7;
        info.rsv1 = (buffer & 0b0100'0000) >> 6;
        info.rsv2 = (buffer & 0b0010'0000) >> 5;
        info.rsv3 = (buffer & 0b0001'0000) >> 4;
        info.opCode = (buffer & 0b0000'1111);
        // info.mask & info.length decode
        ASSERT_READ(&buffer, sizeof(buffer));
        info.mask = (buffer & 0b1000'0000) >> 7;
        info.length = (buffer & 0b0111'1111);
        if (info.length == 126) {
            uint16_t value;
            ASSERT_READ(&value, sizeof(value));
            value = FromBigEndian16(value);
            info.length = value;
        } else if (info.length == 127) {
            uint64_t value;
            ASSERT_READ(&value, sizeof(value));
            value = FromBigEndian64(value);
            info.length = value;
        }
        total = info.length;
        // info.maskingKey decode
        if (info.mask) {
            ASSERT_READ(&info.maskingKey, sizeof(info.maskingKey));
        }

        while (total) {
            uint8_t buf[MTU_VALUE]{};
            auto need = total >= MTU_VALUE ? MTU_VALUE : total;
            ASSERT_READ(buf, need);
            if (info.mask) {
                auto pBlock = (uint8_t *) &info.maskingKey;
                for (int i = 0; i < need; ++i) {
                    buf[i] ^= pBlock[i % 4];
                }
            }
            session->buffer.write(buf, need);
            total -= need;
        }
    }

    if (info.opCode == SESE_WS_OPCODE_TEXT) {
        event->onMessage(this, session);
    } else if (info.opCode == SESE_WS_OPCODE_BIN) {
        event->onBinary(this, session);
    } else if (info.opCode == SESE_WS_OPCODE_CLOSE) {
        event->onClose(this, session);
        doClose(conn, session);
    } else {
        doPong(conn, info, session);
    }
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
        event->onTimeout(this, iterator->second);
        // doClose(conn, iterator->second);
        delete iterator->second;
        sessionMap.erase(iterator);
    }
    HttpService::onTimeout(timeoutEvent);
}

void buildWebsocketFrame(
        sese::net::http::HttpConnection *conn,
        sese::net::ws::FrameHeaderInfo &info,
        sese::net::ws::WebsocketSession *session,
        uint8_t type
) {
    SEND_BUFFER.freeCapacity();
    uint8_t buffer[MTU_VALUE];
    buffer[0] = 0b1000'0000 | (0b0000'1111 & type);
    if (info.length < 126) {
        buffer[1] = 0b0111'1111 & info.length;
        SEND_BUFFER.write(buffer, 2);
    } else if (info.length <= UINT16_MAX && info.length >= 126) {
        buffer[1] = 0b0111'1111 & 126;
        SEND_BUFFER.write(buffer, 2);
        uint16_t value = ToBigEndian16((uint16_t) info.length);
        SEND_BUFFER.write(&value, sizeof(value));
    } else {
        /// 最大帧长度 UINT64_MAX
        buffer[1] = 0b0111'1111 & 127;
        SEND_BUFFER.write(buffer, 2);
        uint64_t value = ToBigEndian64(info.length);
        SEND_BUFFER.write(&value, sizeof(value));
    }

    while (true) {
        auto l = session->buffer.read(buffer, MTU_VALUE);
        if (l == 0) {
            break;
        }
        SEND_BUFFER.write(buffer, l);
    }
}

void sese::service::WebsocketService::doWriteMessage(net::ws::WebsocketSession *session) noexcept {
    auto conn = SESSION_CONN;
    auto info = sese::net::ws::FrameHeaderInfo{};
    info.fin = true;
    info.rsv1 = false;
    info.rsv2 = false;
    info.rsv3 = false;
    info.mask = false;
    info.length = session->buffer.getLength();

    buildWebsocketFrame(conn, info, session, SESE_WS_OPCODE_TEXT);
    HttpService::onControllerWrite(conn->event);
}

void sese::service::WebsocketService::doWriteBinary(net::ws::WebsocketSession *session) noexcept {
    auto conn = SESSION_CONN;
    auto info = sese::net::ws::FrameHeaderInfo{};
    info.fin = true;
    info.rsv1 = false;
    info.rsv2 = false;
    info.rsv3 = false;
    info.mask = false;
    info.length = session->buffer.getLength();

    buildWebsocketFrame(conn, info, session, SESE_WS_OPCODE_BIN);
    HttpService::onControllerWrite(conn->event);
}

void sese::service::WebsocketService::doPong(net::http::HttpConnection *conn, net::ws::FrameHeaderInfo &info, net::ws::WebsocketSession *session) noexcept {
    buildWebsocketFrame(conn, info, session, SESE_WS_OPCODE_PONG);
    HttpService::onControllerWrite(conn->event);
}

void sese::service::WebsocketService::doClose(net::http::HttpConnection *conn, net::ws::WebsocketSession *session) noexcept {
    auto info = sese::net::ws::FrameHeaderInfo{};
    info.fin = true;
    info.rsv1 = false;
    info.rsv2 = false;
    info.rsv3 = false;
    info.mask = false;
    info.length = 0;

    buildWebsocketFrame(conn, info, session, SESE_WS_OPCODE_CLOSE);
    HttpService::onControllerWrite(conn->event);
}

void sese::service::WebsocketService::onProcClose(sese::event::BaseEvent *event) noexcept {
    auto conn = (sese::net::http::HttpConnection *) event->data;
    auto iterator = sessionMap.find(conn);
    if (iterator != sessionMap.end()) {
        delete iterator->second;
        sessionMap.erase(iterator);
    }
    HttpService::onProcClose(event);
}
