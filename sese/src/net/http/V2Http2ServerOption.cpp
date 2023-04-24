#include "sese/net/http/V2Http2ServerOption.h"
#include "sese/net/http/Http2FrameInfo.h"
#include "sese/util/Endian.h"

using namespace sese::net::http;
using namespace sese::net::v2::http;

Http2ServerOption::Http2ServerOption(size_t handleThreads) noexcept
    : ServerOption(), threadPool("Http2Serv", handleThreads) {
}

Http2ServerOption::~Http2ServerOption() noexcept {
    threadPool.shutdown();
}

void Http2ServerOption::onConnect(sese::net::v2::IOContext &ctx) noexcept {
}

void Http2ServerOption::onHandle(sese::net::v2::IOContext &ctx) noexcept {
    auto connIterator = connMap.find(ctx.getIdent());
    if (connIterator == connMap.end()) {
        onHttpHandle(ctx);
    } else {
        onHttp2Handle(ctx, connIterator->second);
    }
}

void Http2ServerOption::onClosing(sese::net::v2::IOContext &ctx) noexcept {
}

void Http2ServerOption::onHttpHandle(sese::net::v2::IOContext &ctx) noexcept {
}

void Http2ServerOption::onHttp2Handle(sese::net::v2::IOContext &ctx, net::http::Http2Connection::Ptr conn) noexcept {
    Http2FrameInfo frame{};
    while (true) {
        if (!readFrame(ctx, frame)) {
            ctx.close();
            return;
        }

        // SETTINGS 帧
        if (frame.ident == 0) {
            if (frame.type == FRAME_TYPE_SETTINGS) {
                // 为当前连接更改设置
            } else {
                // 非法帧
                sendGoaway(ctx, frame.ident, GOAWAY_PROTOCOL_ERROR);
                ctx.close();
                break;
            }
        }

        auto stream = conn->find(frame.ident);
        if (!stream) {
            stream = std::make_shared<Http2Stream>();
            conn->addStream(frame.ident, stream);
        }

        // HEADER 帧
        if (frame.type == FRAME_TYPE_HEADERS) {
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
                // Content-Length 不为 0 则需要接收 data 帧
                break;
            }
        }
        // DATA 帧
        else if (frame.type == FRAME_TYPE_DATA) {
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
                // 触发请求解析 - 任务提交
                break;
            }
        } else {
            // 不处理
        }
    }
}

bool Http2ServerOption::readFrame(IOContext &ctx, sese::net::http::Http2FrameInfo &frame) noexcept {
    uint8_t buffer[9]{};
    auto read = ctx.read(buffer, 9);
    if (read != 9) return false;

    memcpy(&frame.length, buffer + 0, 3);
    memcpy(&frame.type, buffer + 3, 1);
    memcpy(&frame.flags, buffer + 4, 1);
    memcpy(&frame.ident, buffer + 5, 4);

    frame.length = FromBigEndian32(frame.length);
    frame.ident = FromBigEndian32(frame.ident);
    return true;
}

void Http2ServerOption::sendGoaway(sese::net::v2::IOContext &ctx, uint32_t sid, uint32_t eid) noexcept {
    int32_t buffer[2];
    buffer[0] = ToBigEndian32(sid) >> 1;// NOLINT
    buffer[1] = ToBigEndian32(eid);     // NOLINT
    ctx.write(&buffer, 2 * sizeof(uint32_t));
}