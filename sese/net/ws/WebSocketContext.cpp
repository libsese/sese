#include "sese/net/ws/WebSocketContext.h"
#include "sese/util/Endian.h"

sese::net::ws::WebSocketContext::WebSocketContext(sese::Stream *stream) noexcept {
    this->stream = stream;
}

int64_t sese::net::ws::WebSocketContext::read(void *buffer, size_t length) {
    int64_t total = 0;
    auto p = (char *) buffer;
    while (true) {
        if (length <= info.length - readed) {
            // 当前帧足够读写
            auto rt = autoRead(p + total, length);
            if (rt <= 0) {
                return -1;
            }
            readed += rt;
            total += rt;
            return total;
        } else if (length > info.length - readed) {
            // 当前帧不足够读写
            if (info.length - readed > 0) {
                // 有剩余部分
                auto rt = autoRead(p + total, info.length - readed);
                if (rt <= 0) {
                    return -1;
                }
                readed += rt;
                total += rt;
                length -= rt;
            }

            if (info.fin) {
                // 当前已经是最后一帧
                return total;
            }

            readed = 0;
            if (!readInfo()) {
                return -1;
            } else {
                if (info.opCode != SESE_WS_OPCODE_CONT) {
                    // 通常不应触发
                    return -1;
                }
            }
        }
    }
}

int64_t sese::net::ws::WebSocketContext::write(const void *buffer, size_t length) {
    int64_t total = 0;
    FrameHeaderInfo header;
    header.opCode = SESE_WS_OPCODE_TEXT;
    auto p = (const char *) buffer;
    while (length > 1024) {
        header.length = 1024;
        if (!writeInfo(header)) {
            return -1;
        }
        auto rt = stream->write(p + total, 1024);
        if (rt <= 0) {
            return -1;
        }

        total += rt;
        length -= rt;
        header.opCode = SESE_WS_OPCODE_CONT;
    }

    if (length == 0) {
        return total;
    }
    if (!writeInfo(header)) {
        return -1;
    }
    auto rt = stream->write(p + total, length);
    if (rt <= 0) {
        return -1;
    } else {
        total += rt;
        return total;
    }
}

int64_t sese::net::ws::WebSocketContext::writeRaw(const void *buffer, size_t length) noexcept {
    return stream->write(buffer, length);
}

void sese::net::ws::WebSocketContext::close() {
    stream->close();
}

bool sese::net::ws::WebSocketContext::readInfo() noexcept {
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

        autoRead = [this](void *buf, size_t len) -> int64_t { return this->readWithMasking(info.maskingKey, buf, len); };
    } else {
        autoRead = [this](void *buf, size_t len) -> int64_t { return this->read(buf, len); };
    }

    return true;
    // Payload data continued
}

bool sese::net::ws::WebSocketContext::writeInfo(const FrameHeaderInfo &info) noexcept {
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
        uint16_t len = ToBigEndian16(info.length);
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

int64_t sese::net::ws::WebSocketContext::readWithMasking(uint32_t maskingKey, void *buffer, size_t len) noexcept {
    uint8_t temp[1024];
    auto readLength = stream->read(temp, len);
    if (readLength > 0) {
        for (int i = 0; i < readLength; ++i) {
            ((uint8_t *) buffer)[i] = temp[i] ^ ((const uint8_t *) &maskingKey)[i % 4];
        }
    }
    return readLength;
}

int64_t sese::net::ws::WebSocketContext::writeWithMasking(uint32_t maskingKey, const void *buffer, size_t len) noexcept {
    uint8_t temp[1024];
    for (int i = 0; i < len; ++i) {
        temp[i] = ((uint8_t *) buffer)[i] ^ ((const uint8_t *) &maskingKey)[i % 4];
    }
    return stream->write(temp, len);
}

bool sese::net::ws::WebSocketContext::ping() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PING;
    return stream->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocketContext::pong() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PONG;
    return stream->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocketContext::closeNoError() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_CLOSE;
    return stream->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocketContext::closeWithError(const void *error, size_t length) noexcept {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    if (error && length > 0) {
        info.length = length;
        if (!this->writeInfo(info)) {
            return false;
        }
        return stream->write(error, length) == length;
    } else {
        return this->writeInfo(info);
    }
}

bool sese::net::ws::WebSocketContext::closeWithError(const void *error, size_t length, uint32_t maskingKey) noexcept {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    if (error && length > 0) {
        info.length = length;
        info.mask = true;
        info.maskingKey = maskingKey;
        if (!this->writeInfo(info)) {
            return false;
        }

        if (info.mask) {
            return writeWithMasking(maskingKey, error, length) != length;
        } else {
            return stream->write(error, length) == length;
        }
    } else {
        return this->writeInfo(info);
    }
}