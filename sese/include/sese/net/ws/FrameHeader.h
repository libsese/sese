/// \file FrameHeader.h
/// \author kaoru
/// \date 日期
/// \brief Websocket 帧定义
/// \version 0.1

#pragma once

#include "sese/Config.h"

namespace sese::net::ws {
    /// Websocket 帧信息
    struct FrameHeaderInfo;
}

#define SESE_WS_OPCODE_CONT 0x0
#define SESE_WS_OPCODE_TEXT 0x1
#define SESE_WS_OPCODE_BIN 0x2
#define SESE_WS_OPCODE_CLOSE 0x8
#define SESE_WS_OPCODE_PING 0x9
#define SESE_WS_OPCODE_PONG 0xa

struct API sese::net::ws::FrameHeaderInfo {
    bool fin = false;
    bool rsv1 = false;
    bool rsv2 = false;
    bool rsv3 = false;
    uint8_t opCode = 0;
    bool mask = false;
    uint64_t length = 0;
    uint32_t maskingKey = 0;
};