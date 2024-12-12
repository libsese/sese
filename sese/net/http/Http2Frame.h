// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Http2Frame.h
 * @author kaoru
 * @version 0.1
 * @brief HTTP 2 Frame Information
 * @date September 13, 2023
 */

#pragma once

#include <cstdint>
#include <memory>

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

constexpr static uint16_t SETTINGS_HEADER_TABLE_SIZE = 0x1;
constexpr static uint16_t SETTINGS_ENABLE_PUSH = 0x2;
constexpr static uint16_t SETTINGS_MAX_CONCURRENT_STREAMS = 0x3;
constexpr static uint16_t SETTINGS_INITIAL_WINDOW_SIZE = 0x4;
constexpr static uint16_t SETTINGS_MAX_FRAME_SIZE = 0x5;
constexpr static uint16_t SETTINGS_MAX_HEADER_LIST_SIZE = 0x6;

constexpr static uint8_t SETTINGS_FLAGS_ACK = 0x1;

constexpr static auto MAGIC_STRING = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

/// HTTP 2 Frame Information
struct Http2FrameInfo {
    uint32_t length;
    uint8_t type;
    uint8_t flags;
    uint32_t ident;
};

/// HTTP 2 Frame Information (including buffer)
struct Http2Frame {
    using Ptr = std::unique_ptr<Http2Frame>;

    uint32_t length;
    uint8_t type;
    uint8_t flags;
    uint32_t ident;

    std::unique_ptr<char []> frame;

    explicit Http2Frame(size_t frame_size);

    /// Get buffer including frame header
    /// @return Buffer
    [[nodiscard]] char *getFrameBuffer() const;

    /// Get buffer length including frame header
    /// @return Buffer length
    [[nodiscard]] size_t getFrameLength() const;

    /// Get buffer excluding frame header
    /// @return Buffer
    [[nodiscard]] char *getFrameContentBuffer() const;

    /// Get buffer length excluding frame header
    /// @return Buffer length
    [[nodiscard]] size_t getFrameContentLength() const;

    /// Build frame header based on existing information
    void buildFrameHeader() const;
};


} // namespace sese::net::http