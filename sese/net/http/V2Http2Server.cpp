#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/http/V2Http2Server.h"
#include "sese/net/http/Http2FrameInfo.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/util/Endian.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/text/StringBuilder.h"
#include "sese/convert/Base64Converter.h"

#include <chrono>

using namespace sese::net::http;
using namespace sese::net::v2::http;

Http2Context::Http2Context(const net::v2::http::Http2Stream::Ptr &stream, net::http::DynamicTable &table) noexcept : table(table), stream(stream) {}

int64_t Http2Context::write(const void *buffer, size_t length) {
    if (0 == data) {
        // 需要先压缩头部
        header = this->flushHeader();
    }
    data += length;
    return stream->buffer.write(buffer, length);
}

int64_t Http2Context::read(void *buffer, size_t length) {
    return stream->buffer.read(buffer, length);
}

void Http2Context::setOnce(const std::string &key, const std::string &value) noexcept {
    this->stream->responseOnceHeader.set(key, value);
}

void Http2Context::setIndexed(const std::string &key, const std::string &value) noexcept {
    this->stream->responseIndexedHeader.set(key, value);
}

void Http2Context::get(const std::string &key, const std::string &def) noexcept {
    stream->requestHeader.get(key, def);
}

size_t Http2Context::flushHeader() noexcept {
    return HPackUtil::encode(stream.get(), table, stream->responseOnceHeader, stream->responseIndexedHeader);
}

void Http2Server::onHandle(sese::net::v2::IOContext &ctx) noexcept {
    mutex.lock();
    auto connIterator = connMap.find(ctx.getIdent());
    if (connIterator == connMap.end()) {
        mutex.unlock();

        char buffer[32]{};
        ctx.peek(buffer, 31);
        std::string_view view{buffer, 31};
        if (view.find("HTTP/1.1") == std::string::npos) {
            auto conn = std::make_shared<Http2Connection>(ctx);

            mutex.lock();
            connMap[ctx.getIdent()] = conn;
            mutex.unlock();

            onHttp2Handle(conn, false);
        } else {
            onHttpHandle(ctx);
        }

    } else {
        mutex.unlock();
        onHttp2Handle(connIterator->second, false);
    }
}

void Http2Server::onClosing(sese::net::v2::IOContext &ctx) noexcept {
    mutex.lock();
    auto iterator = connMap.find(ctx.getIdent());
    if (iterator != connMap.end()) {
        connMap.erase(iterator);
    }
    mutex.unlock();
}

void Http2Server::onHttpHandle(sese::net::v2::IOContext &ctx) noexcept {
    auto httpContext = HttpContext();
    httpContext.reset(&ctx);

    if (!sese::net::http::HttpUtil::recvRequest(&httpContext, &httpContext.request)) {
        httpContext.close();
        return;
    }

    // 比较常量
    const char *SSL = "h2";
    const char *NoSSL = "h2c";
    const char *UPGRADE_CMP_STR = SSL;
    if (!sslContext) {
        UPGRADE_CMP_STR = NoSSL;
    }
    // 状态变量
    bool upgrade = false;
    bool http2 = false;
    bool settings = false;
    // 确认状态
    auto connectString = httpContext.request.get("Connection", "undef");
    auto upgradeString = httpContext.request.get("Upgrade", "undef");
    if (connectString != "undef") {
        auto connectVector = text::StringBuilder::split(connectString, ", ");
        for (decltype(auto) str: connectVector) {
            if (strcasecmp(str.c_str(), "Upgrade") == 0) {
                upgrade = true;
            } else if (strcasecmp(str.c_str(), "Http2-Settings") == 0) {
                settings = true;
            }
        }
    }
    if (upgradeString != "undef") {
        if (strcasecmp(upgradeString.c_str(), UPGRADE_CMP_STR) == 0) {
            http2 = true;
        }
    }

    Http2Connection::Ptr conn;
    Http2Stream::Ptr stream;
    // 对状态反应
    auto ok = false;
    if (upgrade && http2) {
        // 确定升级至 HTTP2
        conn = std::make_shared<Http2Connection>(ctx);
        if (settings) {
            // 确定存在 HTTP2-Settings 字段

            mutex.lock();
            connMap[ctx.getIdent()] = conn;
            mutex.unlock();

            auto settingsStr = httpContext.request.get("Http2-Settings", "undef");
            if (settingsStr != "undef") {
                stream = std::make_shared<Http2Stream>();
                stream->sid = 1;
                header2Http2(httpContext, stream->requestHeader);
                conn->streamMap[1] = stream;

                char buffer[6];
                auto ident = (uint16_t *) &buffer[0];
                auto value = (uint32_t *) &buffer[2];

                auto input = sese::io::InputBufferWrapper(settingsStr.c_str(), settingsStr.size());
                auto output = sese::io::ByteBuilder(1024);
                sese::Base64Converter::decode(&input, &output);
                int64_t len = 0;
                while ((len = output.read(buffer, 6)) == 6) {
                    *ident = FromBigEndian16(*ident);
                    *value = FromBigEndian32(*value);

                    switch (*ident) {
                        case SETTINGS_HEADER_TABLE_SIZE:
                            conn->dynamicTable4recv.resize(*value);
                            conn->headerTableSize = *value;
                            break;
                        case SETTINGS_MAX_CONCURRENT_STREAMS:
                            conn->maxConcurrentStream = (*value == 0 ? 100 : *value);
                            break;
                        case SETTINGS_MAX_FRAME_SIZE:
                            if (*value > 16777215) {
                                sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                                conn->context.close();
                            } else {
                                conn->maxFrameSize = *value;
                            }
                            break;
                        case SETTINGS_ENABLE_PUSH:
                            if (*value <= 1) {
                                conn->enablePush = *value;
                            } else {
                                sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                                conn->context.close();
                            }
                            break;
                        case SETTINGS_MAX_HEADER_LIST_SIZE:
                            conn->maxHeaderListSize = *value;
                            break;
                        case SETTINGS_INITIAL_WINDOW_SIZE:
                            if (*value > 2147483647) {
                                sendGoaway(conn, 0, GOAWAY_FLOW_CONTROL_ERROR);
                                conn->context.close();
                            } else {
                                conn->windowSize = *value;
                            }
                            break;
                        default:
                            // 暂不处理
                            break;
                    }
                }
                if (len == 0) {
                    ok = true;
                } else {
                    // 不完整 - 暂不处理
                    ok = false;
                }
            }
        } else {
            ok = true;
        }
    }
    // 发送回报
    if (ok) {
        // Switching Protocols
        httpContext.response.setCode(101);
        httpContext.response.set("Connection", "Upgrade");
        httpContext.response.set("Upgrade", UPGRADE_CMP_STR);
        httpContext.flush();
        // 放弃当前时间片
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        onHttp2Handle(conn, true);
    } else {
        char resp[] = {"Only support Http/2"};
        httpContext.response.setCode(403);
        httpContext.response.set("Content-Length", std::to_string(sizeof(resp) - 1));
        httpContext.response.set("Connection", "Close");
        httpContext.flush();
        httpContext.write(resp, sizeof(resp) - 1);
        httpContext.close();
    }
}

void Http2Server::onHttp2Handle(const net::v2::http::Http2Connection::Ptr &conn, bool first) noexcept {
    IOContext &ctx = conn->context;
    Http2FrameInfo frame{};
    frame.type = FRAME_TYPE_SETTINGS;
    writeFrame(conn, frame);

    if (first) {
        char buffer[MAGIC_STRING.length() + 1]{};
        ctx.read(buffer, MAGIC_STRING.length());
        if (MAGIC_STRING != buffer) {
            ctx.close();
            return;
        } else {
            threadPool->postTask([this, conn, stream = conn->streamMap[1]]() {
                this->onResponseAndSend(conn, stream);
            });
        }
    }

    while (true) {
        auto ret = readFrame(ctx, frame);
        if (-1 == ret) {
            ctx.close();
            return;
        } else if (ret == 0) {
            return;
        }

        if (frame.type == FRAME_TYPE_SETTINGS) {
            // SETTINGS 帧
            if (frame.ident != 0) {
                sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                ctx.close();
                break;
            }
            onSettingsFrame(frame, conn);
            sendACK(conn);
            continue;
        } else if (frame.type == FRAME_TYPE_WINDOW_UPDATE) {
            // WINDOW_UPDATE 帧
            onWindowUpdateFrame(frame, conn);
            continue;
        } else if (frame.type == FRAME_TYPE_HEADERS) {
            // HEADER 帧
            onHeadersFrame(frame, conn);
            continue;
        } else if (frame.type == FRAME_TYPE_CONTINUATION) {
            // HEADER 帧
            onHeadersFrame(frame, conn);
            continue;
        } else if (frame.type == FRAME_TYPE_DATA) {
            // DATA 帧
            onDataFrame(frame, conn);
            continue;
        } else if (frame.type == FRAME_TYPE_RST_STREAM) {
            // RST_STREAM 帧
            continue;
        } else {
            // FRAME_TYPE_PRIORITY 帧，不处理
            // 不处理的帧或者非法帧
            // sendGoaway(conn, frame.ident, GOAWAY_PROTOCOL_ERROR);
            // ctx.close();
            continue;
        }
    }
}

void Http2Server::onHttp2Request(Http2Context &h2Ctx) noexcept {
    const std::string content = "<h1>Default implementation from Http2Server</h1>";

    h2Ctx.setIndexed("server", "sese::net::v2::http::Http2Server");
    h2Ctx.setOnce(":status", "200");
    h2Ctx.setOnce("content-length", std::to_string(content.length()));

    h2Ctx.write(content.c_str(), content.length());
}

int64_t Http2Server::readFrame(IOContext &ctx, sese::net::http::Http2FrameInfo &frame) noexcept {
    uint8_t buffer[9]{};
    auto read = ctx.read(buffer, 9);
    if (read == 0) {
        return 0;
    } else if (read != 9) {
        if (errno == 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        } else {
            return -1;
        }
    }

    memset(&frame, 0, sizeof(frame));
    memcpy(((char *) &frame.length) + 1, buffer + 0, 3);
    memcpy(&frame.type, buffer + 3, 1);
    memcpy(&frame.flags, buffer + 4, 1);
    memcpy(&frame.ident, buffer + 5, 4);

    frame.length = FromBigEndian32(frame.length);
    frame.ident = FromBigEndian32(frame.ident);
    return 1;
}

int64_t Http2Server::writeFrame(const Http2Connection::Ptr &conn, sese::net::v2::http::Http2Server::Http2FrameInfo &info) noexcept {
    auto len = ToBigEndian32(info.length);
    auto ident = ToBigEndian32(info.ident);

    char buffer[9];
    memcpy(buffer + 0, ((const char *) &len) + 1, 3);
    memcpy(buffer + 3, &info.type, 1);
    memcpy(buffer + 4, &info.flags, 1);
    memcpy(buffer + 5, &ident, 4);

    return conn->context.write(buffer, 9);
}

void Http2Server::sendGoaway(const Http2Connection::Ptr &conn, uint32_t sid, uint32_t eid) noexcept {
    conn->mutex.lock();
    int32_t buffer[2];
    buffer[0] = ToBigEndian32(sid) >> 1;// NOLINT
    buffer[1] = ToBigEndian32(eid);     // NOLINT
    conn->context.write(&buffer, 2 * sizeof(uint32_t));
    conn->mutex.unlock();
}

void Http2Server::sendACK(const Http2Connection::Ptr &conn) noexcept {
    conn->mutex.lock();
    uint8_t buffer[]{0x0, 0x0, 0x0, 0x4, 0x1, 0x0, 0x0, 0x0, 0x0};
    conn->context.write(buffer, 9);
    conn->mutex.unlock();
}

void Http2Server::header2Http2(HttpContext &ctx, Header &header) noexcept {
    header = ctx.request;
    // 此处做转换
#define STR(str) #str
#define XX(type)                          \
    case RequestType::type:               \
        header.set(":method", STR(type)); \
        break

    switch (ctx.request.getType()) {
        XX(Options);
        XX(Get);
        XX(Post);
        XX(Head);
        XX(Put);
        XX(Delete);
        XX(Trace);
        XX(Connect);
        XX(Another);
    }
#undef XX
#undef STR
    header.set(":path", ctx.request.getUrl());
}


void Http2Server::onSettingsFrame(sese::net::v2::http::Http2Server::Http2FrameInfo &frame, const Http2Connection::Ptr &conn) noexcept {
    IOContext &ctx = conn->context;
    // 为当前连接更改设置
    io::ByteBuilder builder;
    char buf[1024];
    size_t length = 0;
    while (length < frame.length) {
        auto need = frame.length - length > 1024 ? 1024 : frame.length - length;
        auto l = ctx.read(buf, need);
        if (l > 0) {
            builder.write(buf, l);
            length += l;
        } else {
            ctx.close();
            break;
        }
    }

    char buffer[6];
    auto ident = (uint16_t *) &buffer[0];
    auto value = (uint32_t *) &buffer[2];

    int64_t len = 0;
    while ((len = builder.read(buffer, 6)) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case SETTINGS_HEADER_TABLE_SIZE:
                conn->dynamicTable4recv.resize(*value);
                break;
            case SETTINGS_MAX_CONCURRENT_STREAMS:
            case SETTINGS_MAX_FRAME_SIZE:
            case SETTINGS_ENABLE_PUSH:
            case SETTINGS_MAX_HEADER_LIST_SIZE:
            case SETTINGS_INITIAL_WINDOW_SIZE:
            default:
                // 按照标准忽略该设置
                break;
        }
    }
}

void Http2Server::onWindowUpdateFrame(sese::net::v2::http::Http2Server::Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept {
    // 不做控制 - 读取负载
    uint32_t data;
    conn->context.read(&data, sizeof(data));
}

void Http2Server::onHeadersFrame(sese::net::v2::http::Http2Server::Http2FrameInfo &frame, const Http2Connection::Ptr &conn) noexcept {
    auto stream = conn->find(frame.ident);
    if (!stream) {
        stream = std::make_shared<Http2Stream>();
        stream->sid = frame.ident;
        conn->addStream(frame.ident, stream);
    }

    IOContext &ctx = conn->context;
    char buffer[1024];
    size_t length = 0;
    while (length < frame.length) {
        auto need = frame.length - length > 1024 ? 1024 : frame.length - length;
        auto l = ctx.read(buffer, need);
        if (l > 0) {
            stream->buffer.write(buffer, l);
            length += l;
        } else {
            ctx.close();
            break;
        }
    }
    if (frame.flags == FRAME_FLAG_END_HEADERS) {
        // 触发头解析
        if (!HPackUtil::decode(stream.get(), frame.length, conn->dynamicTable4recv, stream->requestHeader)) {
            // 解析失败关闭整个连接
            ctx.close();
        }
        // Content-Length 不为 0 则需要接收 data 帧
        if (stream->requestHeader.get("Content-Length", "undef") == "undef") {
            threadPool->postTask([this, conn, stream]() {
                onResponseAndSend(conn, stream);
            });
        }
    }
}

void Http2Server::onDataFrame(sese::net::v2::http::Http2Server::Http2FrameInfo &frame, const Http2Connection::Ptr &conn) noexcept {
    auto stream = conn->find(frame.ident);
    if (!stream) {
        stream = std::make_shared<Http2Stream>();
        stream->sid = frame.ident;
        conn->addStream(frame.ident, stream);
    }

    IOContext &ctx = conn->context;

    char buffer[1024];
    size_t length = 0;
    while (length < frame.length) {
        auto need = frame.length - length > 1024 ? 1024 : frame.length - length;
        auto l = ctx.read(buffer, need);
        if (l > 0) {
            stream->buffer.write(buffer, l);
            length += l;
        } else {
            ctx.close();
            break;
        }
    }
    if (frame.flags == FRAME_FLAG_END_STREAM) {
        threadPool->postTask([this, conn, stream]() {
            onResponseAndSend(conn, stream);
        });
    }
}

void Http2Server::onResponseAndSend(const Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept {
    Http2Context ctx(stream, conn->dynamicTable4recv);
    onHttp2Request(ctx);

    // 发送 Header 帧
    conn->mutex.lock();
    sendHeader(ctx, conn, stream);
    if (ctx.data > 0) {
        sendData(ctx, conn, stream);
    }
    conn->mutex.unlock();
}

void Http2Server::sendHeader(Http2Context &ctx, const sese::net::v2::http::Http2Server::Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept {
    bool first = true;
    size_t headerSize = ctx.header;
    io::ByteBuilder &builder = ctx.stream->buffer;

    char buffer[1024];
    Http2FrameInfo info{};
    info.ident = stream->sid;

    while (true) {
        auto req = headerSize >= 1024 ? 1024 : headerSize;
        headerSize -= req;
        builder.read(buffer, req);

        if (first) {
            info.type = FRAME_TYPE_HEADERS;
            first = false;
        } else {
            info.type = FRAME_TYPE_CONTINUATION;
        }

        if (headerSize == 0) {
            info.flags = FRAME_FLAG_END_HEADERS;
        }

        info.length = (uint32_t) req;
        writeFrame(conn, info);
        conn->context.write(buffer, req);

        if (headerSize == 0) {
            break;
        }
    }
}

void Http2Server::sendData(sese::net::v2::http::Http2Context &ctx, const Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept {
    size_t dataSize = ctx.data;
    io::ByteBuilder &builder = ctx.stream->buffer;

    char buffer[1024];
    Http2FrameInfo info{};
    info.type = FRAME_TYPE_DATA;
    info.ident = stream->sid;

    while (true) {
        auto req = dataSize >= 1024 ? 1024 : dataSize;
        dataSize -= req;
        builder.read(buffer, req);

        if (dataSize == 0) {
            info.flags = FRAME_FLAG_END_STREAM;
        }

        info.length = (uint32_t) req;
        writeFrame(conn, info);
        conn->context.write(buffer, req);

        if (dataSize == 0) {
            break;
        }
    }
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif