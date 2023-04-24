#pragma once

#include <cstdint>

namespace sese::net::http {

    constexpr static uint8_t FRAME_FLAG_END_STREAM = 0x1;
    constexpr static uint8_t FRAME_FLAG_END_HEADERS = 0x4;
    constexpr static uint8_t FRAME_FLAG_PADDED = 0x8;
    constexpr static uint8_t FRAME_FLAG_PRIORITY = 0x20;

    constexpr static uint8_t FRAME_TYPE_DATA = 0x0;
    constexpr static uint8_t FRAME_TYPE_HEADERS = 0x1;
    constexpr static uint8_t FRAME_TYPE_PRIORITY = 0x2;
    constexpr static uint8_t FRAME_TYPE_RST_STREAM = 0x3;
    constexpr static uint8_t FRAME_TYPE_SETTINGS = 0x4;
    constexpr static uint8_t FRAME_TYPE_PUSH_PROMISE = 0x5;
    constexpr static uint8_t FRAME_TYPE_PING = 0x6;
    constexpr static uint8_t FRAME_TYPE_GOAWAY = 0x7;
    constexpr static uint8_t FRAME_TYPE_WINDOW_UPDATE = 0x8;
    constexpr static uint8_t FRAME_TYPE_CONTINUATION = 0x9;
    constexpr static uint8_t FRAME_TYPE_ALTSVC = 0xa;
    constexpr static uint8_t FRAME_TYPE_ORIGIN = 0xc;

    constexpr static uint8_t GOAWAY_NO_ERROR = 0x0;
    constexpr static uint8_t GOAWAY_PROTOCOL_ERROR = 0x1;
    constexpr static uint8_t GOAWAY_INTERNAL_ERROR = 0x2;
    constexpr static uint8_t GOAWAY_FLOW_CONTROL_ERROR = 0x3;
    constexpr static uint8_t GOAWAY_SETTINGS_TIMEOUT = 0x4;
    constexpr static uint8_t GOAWAY_STREAM_CLOSED = 0x5;
    constexpr static uint8_t GOAWAY_FRAME_SIZE_ERROR = 0x6;
    constexpr static uint8_t GOAWAY_REFUSED_STREAM = 0x7;
    constexpr static uint8_t GOAWAY_CANCEL = 0x8;
    constexpr static uint8_t GOAWAY_COMPRESSION_ERROR = 0x9;
    constexpr static uint8_t GOAWAY_CONNECT_ERROR = 0xa;
    constexpr static uint8_t GOAWAY_ENHANCE_YOUR_CALM = 0xb;
    constexpr static uint8_t GOAWAY_INADEQUATE_SECURITY = 0xc;
    constexpr static uint8_t GOAWAY_HTTP_1_1_REQUIRED = 0xd;

    struct Http2FrameInfo {
        uint32_t length{0};
        uint8_t type{0};
        uint8_t flags{0};
        uint32_t ident{0};
    };

}// namespace sese::net::http