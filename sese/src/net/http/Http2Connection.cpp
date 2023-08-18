#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif _WIN32
#pragma warning(disable : 4996)
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/http/Http2Connection.h"

using namespace sese::net::v2::http;

Http2Connection::Http2Connection(v2::IOContext ctx) noexcept : context(ctx) {}

void Http2Connection::addStream(uint32_t sid, const Http2Stream::Ptr &stream) noexcept {
    streamMap[sid] = stream;
}

Http2Stream::Ptr Http2Connection::find(uint32_t sid) {
    auto iterator = streamMap.find(sid);
    if (iterator != streamMap.end()) {
        return iterator->second;
    } else {
        return nullptr;
    }
}

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#include <sese/convert/Base64Converter.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/util/ByteBuilder.h>
#include <sese/net/http/Http2FrameInfo.h>

void sese::net::http::Http2Connection::decodeHttp2Settings(const std::string &settings) noexcept {
    char buffer[6];
    auto ident = (uint16_t *) &buffer[0];
    auto value = (uint32_t *) &buffer[2];
    auto input = sese::InputBufferWrapper(settings.c_str(), settings.length());
    auto output = sese::ByteBuilder(1024);
    sese::Base64Converter::decode(&input, &output);

    int64_t len = 0;
    while ((len = output.read(buffer, 6)) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case SETTINGS_HEADER_TABLE_SIZE:
                this->dynamicTable1.resize(*value);
                this->headerTableSize = *value;
                break;
            case SETTINGS_MAX_CONCURRENT_STREAMS:
                this->maxConcurrentStream = (*value == 0 ? 100 : *value);
                break;
            case SETTINGS_MAX_FRAME_SIZE:
                if (*value > 16777215) {
                    // sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                    // conn->context.close();
                } else {
                    this->maxFrameSize = *value;
                }
                break;
            case SETTINGS_ENABLE_PUSH:
                if (*value <= 1) {
                    this->enablePush = *value;
                } else {
                    // sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                    // conn->context.close();
                }
                break;
            case SETTINGS_MAX_HEADER_LIST_SIZE:
                this->maxHeaderListSize = *value;
                break;
            case SETTINGS_INITIAL_WINDOW_SIZE:
                if (*value > 2147483647) {
                    // sendGoaway(conn, 0, GOAWAY_FLOW_CONTROL_ERROR);
                    // conn->context.close();
                } else {
                    this->windowSize = *value;
                }
                break;
            default:
                // 暂不处理
                break;
        }
    }
}