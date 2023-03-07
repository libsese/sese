#include "sese/net/ws/WebSocketContext.h"
#include "sese/util/Endian.h"

#include <sstream>

sese::net::ws::WebSocketContext::WebSocketContext(Stream *stream) {
    WebSocketContext::stream = stream;
}

bool sese::net::ws::WebSocketContext::readFrameInfo(FrameHeaderInfo &info) {
    struct FrameHeaderBuffer buffer;
    if (stream->read(&buffer, sizeof(buffer)) != sizeof(buffer)) {
        return false;
    }

    info.fin = buffer.FIN;
    info.rsv1 = buffer.RSV1;
    info.rsv2 = buffer.RSV2;
    info.rsv3 = buffer.RSV3;
    info.mask = buffer.MASK;
    info.opCode = buffer.OPCODE;

    if (buffer.PAYLOAD_LEN <= 125) {
        info.length = buffer.PAYLOAD_LEN;
    } else if (buffer.PAYLOAD_LEN == 126) {
        uint16_t len;
        if (stream->read(&len, sizeof(len)) != sizeof(len)) {
            return false;
        }
        info.length = FromBigEndian16(len);
    } else {
        uint64_t len;
        if (stream->read(&len, sizeof(len)) != sizeof(len)) {
            return false;
        }
        info.length = FromBigEndian64(len);
    }

    if (info.mask) {
        if (stream->read(&info.maskingKey, sizeof(info.maskingKey)) != sizeof(info.maskingKey)) {
            return false;
        }
        // todo 验证大小端转换在此处是否是多余的
        info.maskingKey = FromBigEndian32(info.maskingKey);
    }
    return true;
    // Payload data continued
}

bool sese::net::ws::WebSocketContext::writeFrameInfo(const FrameHeaderInfo &info) {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = info.fin;
    buffer.RSV1 = info.rsv1;
    buffer.RSV2 = info.rsv2;
    buffer.RSV3 = info.rsv3;
    buffer.MASK = info.mask;
    buffer.OPCODE = info.opCode;

    if (info.length <= 125) {
        buffer.PAYLOAD_LEN = info.length;
        if (stream->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
    } else if (info.length > 125 && info.length <= 0xFFFF) {
        buffer.PAYLOAD_LEN = 126;
        if (stream->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
        uint16_t len = ToBigEndian16((uint16_t) info.length);
        if (stream->write(&len, sizeof(uint16_t)) != sizeof(uint16_t)) {
            return false;
        }
    } else {
        buffer.PAYLOAD_LEN = 127;
        if (stream->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
        uint64_t len = ToBigEndian64(info.length);
        if (stream->write(&len, sizeof(uint64_t)) != sizeof(uint64_t)) {
            return false;
        }
    }

    if (info.mask) {
        // todo 验证大小端转换在此处是否是多余的
        uint32_t key = ToBigEndian32(info.maskingKey);
        if (stream->write(&key, sizeof(uint32_t)) != sizeof(info.maskingKey)) {
            return false;
        }
    }

    return true;
    // Payload data continued
}

bool sese::net::ws::WebSocketContext::ping() {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PING;
    if (stream->write(&buffer, sizeof(buffer)) == sizeof(buffer)) {
        return true;
    } else {
        return false;
    }
}

bool sese::net::ws::WebSocketContext::pong() {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PONG;
    if (stream->write(&buffer, sizeof(buffer)) == sizeof(buffer)) {
        return true;
    } else {
        return false;
    }
}

int64_t sese::net::ws::WebSocketContext::readBinary(void *buf, size_t len) {
    return stream->read(buf, len);
}

int64_t sese::net::ws::WebSocketContext::readBinary(void *buf, size_t len, uint32_t maskingKey, size_t &offset) {
    uint8_t temp[1024];
    auto readLength = stream->read(temp, len);
    if (readLength > 0) {
        for (int i = 0; i < readLength; ++i) {
            ((uint8_t *) buf)[i] = temp[i] ^ ((const uint8_t *) &maskingKey)[(offset + i) % 4];
        }
    } else {
        return -1;
    }
    offset += readLength;
    return readLength;
}

int64_t sese::net::ws::WebSocketContext::writeBinary(const void *buf, size_t len) {
    return stream->write(buf, len);
}

int64_t sese::net::ws::WebSocketContext::writeBinary(const void *buf, size_t len, uint32_t maskingKey, size_t &offset) {
    uint8_t temp[1024];
    for (int i = 0; i < len; ++i) {
        temp[i] = ((uint8_t *) buf)[i] ^ ((const uint8_t *) &maskingKey)[(offset + i) % 4];
    }
    auto rt = stream->write(temp, len);
    if (rt != len) {
        return -1;
    } else {
        offset += rt;
        return rt;
    }
}

bool sese::net::ws::WebSocketContext::closeNoError() {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_CLOSE;
    if (stream->write(&buffer, sizeof(buffer)) == sizeof(buffer)) {
        return true;
    } else {
        return false;
    }
}

bool sese::net::ws::WebSocketContext::closeWithError(const void *err, size_t len) {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    if (err && len > 0) {
        info.length = len;
        if (!this->writeFrameInfo(info)) {
            return false;
        }
        if (stream->write(err, len) == len) {
            return true;
        } else {
            return false;
        }
    } else {
        if (this->writeFrameInfo(info)) {
            return true;
        } else {
            return false;
        }
    }
}

bool sese::net::ws::WebSocketContext::closeWithError(const void *err, size_t len, uint32_t maskingKey) {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    if (err && len > 0) {
        info.length = len;
        info.mask = true;
        info.maskingKey = maskingKey;
        if (!this->writeFrameInfo(info)) {
            return false;
        }

        if (info.mask) {
            size_t offset = 0;
            if (writeBinary(err, len, maskingKey, offset) != len) {
                return false;
            }
        } else {
            if (stream->write(err, len) == len) {
                return true;
            } else {
                return false;
            }
        }
    } else {
        if (this->writeFrameInfo(info)) {
            return true;
        } else {
            return false;
        }
    }
}