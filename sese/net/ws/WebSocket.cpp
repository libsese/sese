#include "sese/net/ws/WebSocket.h"
#include "sese/util/Endian.h"

sese::net::ws::WebSocket::WebSocket(sese::Stream *stream) noexcept {
    this->stream = stream;
}

int64_t sese::net::ws::WebSocket::read(void *buffer, size_t length) {
    return stream->read(buffer, length);
}

int64_t sese::net::ws::WebSocket::write(const void *buffer, size_t length) {
    return stream->write(buffer, length);
}

void sese::net::ws::WebSocket::close() {
    stream->close();
}

bool sese::net::ws::WebSocket::readInfo(FrameHeaderInfo &info) noexcept {
    struct FrameHeaderBuffer buffer;
    if (this->read(&buffer, sizeof(buffer)) != sizeof(buffer)) {
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
        if (this->read(&len, sizeof(len)) != sizeof(len)) {
            return false;
        }
        info.length = FromBigEndian16(len);
    } else {
        uint64_t len;
        if (this->read(&len, sizeof(len)) != sizeof(len)) {
            return false;
        }
        info.length = FromBigEndian64(len);
    }

    if (info.mask) {
        if (this->read(&info.maskingKey, sizeof(info.maskingKey)) != sizeof(info.maskingKey)) {
            return false;
        }
        // todo 验证大小端转换在此处是否是多余的
        info.maskingKey = FromBigEndian32(info.maskingKey);
    }

    return true;
    // Payload data continued
}

bool sese::net::ws::WebSocket::writeInfo(const FrameHeaderInfo &info) noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = info.fin;
    buffer.RSV1 = info.rsv1;
    buffer.RSV2 = info.rsv2;
    buffer.RSV3 = info.rsv3;
    buffer.MASK = info.mask;
    buffer.OPCODE = info.opCode;

    if (info.length <= 125) {
        buffer.PAYLOAD_LEN = info.length;
        if (this->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
    } else if (info.length > 125 && info.length < 0xFFFF) {
        buffer.PAYLOAD_LEN = 126;
        if (this->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
        uint16_t len = ToBigEndian16(info.length);
        if (this->write(&len, sizeof(uint16_t)) != sizeof(uint16_t)) {
            return false;
        }
    } else {
        buffer.PAYLOAD_LEN = 127;
        if (this->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
        uint64_t len = ToBigEndian64(info.length);
        if (this->write(&len, sizeof(uint64_t)) != sizeof(uint64_t)) {
            return false;
        }
    }

    if (info.mask) {
        // todo 验证大小端转换在此处是否是多余的
        uint32_t key = ToBigEndian32(info.maskingKey);
        if (this->write(&key, sizeof(uint32_t)) != sizeof(info.maskingKey)) {
            return false;
        }
    }

    return true;
    // Payload data continued
}

int64_t sese::net::ws::WebSocket::readWithMasking(uint32_t maskingKey, void *buffer, size_t len) noexcept {
    uint8_t temp[1024];
    auto readLength = this->read(temp, len);
    if (readLength > 0) {
        for (int i = 0; i < readLength; ++i) {
            ((uint8_t *) buffer)[i] = temp[i] ^ ((const uint8_t *) &maskingKey)[i % 4];
        }
    }
    return readLength;
}

int64_t sese::net::ws::WebSocket::writeWithMasking(uint32_t maskingKey, const void *buffer, size_t len) noexcept {
    uint8_t temp[1024];
    for (int i = 0; i < len; ++i) {
        temp[i] = ((uint8_t *) buffer)[i] ^ ((const uint8_t *) &maskingKey)[i % 4];
    }
    return this->write(temp, len);
}

bool sese::net::ws::WebSocket::ping() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PING;
    return this->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocket::pong() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PONG;
    return this->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocket::closeNoError() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_CLOSE;
    return this->write(&buffer, sizeof(buffer)) == sizeof(buffer);
}

bool sese::net::ws::WebSocket::closeWithError(const void *error, size_t length) noexcept {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    if (error && length > 0) {
        info.length = length;
        if (!this->writeInfo(info)) {
            return false;
        }
        return this->write(error, length) == length;
    } else {
        return this->writeInfo(info);
    }
}

bool sese::net::ws::WebSocket::closeWithError(const void *error, size_t length, uint32_t maskingKey) noexcept {
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
            return this->write(error, length) == length;
        }
    } else {
        return this->writeInfo(info);
    }
}