#pragma once

#include <cstdint>

namespace sese::net::http {

    constexpr static uint8_t END_STREAM = 0x1;
    constexpr static uint8_t END_HEADERS = 0x4;
    constexpr static uint8_t PADDED = 0x8;
    constexpr static uint8_t PRIORITY = 0x20;

    struct Http2FrameInfo {
        enum class Type : uint8_t {
            DATA = 0x0,
            HEADERS = 0x1,
            PRIORITY = 0x2,
            RST_STREAM = 0x3,
            SETTINGS = 0x4,
            PUSH_PROMISE = 0x5,
            PING = 0x6,
            GOAWAY = 0x7,
            WINDOW_UPDATE = 0x8,
            CONTINUATION = 0x9,
            ALTSVC = 0xa,
            ORIGIN = 0xc
        };

        uint32_t length{0};
        Type type{0};
        uint8_t flags{0};
        uint8_t ident{0};
    };

}// namespace sese::net::http