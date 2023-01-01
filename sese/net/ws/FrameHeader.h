#pragma once
#include "sese/Config.h"

namespace sese::net::ws {
    struct FrameHeaderBuffer;
    struct FrameHeaderInfo;
}

#define SESE_WS_OPCODE_CONT 0x0
#define SESE_WS_OPCODE_TEXT 0x1
#define SESE_WS_OPCODE_BIN 0x2
#define SESE_WS_OPCODE_CLOSE 0x8
#define SESE_WS_OPCODE_PING 0x9
#define SESE_WS_OPCODE_PONG 0xa

struct sese::net::ws::FrameHeaderBuffer {
    uint8_t FIN : 1 = 0;
    uint8_t RSV1 : 1 = 0;
    uint8_t RSV2 : 1 = 0;
    uint8_t RSV3 : 1 = 0;
    uint8_t OPCODE : 4 = 0;
    uint8_t MASK : 1 = 0;
    uint8_t PAYLOAD_LEN : 7 = 0;
};

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