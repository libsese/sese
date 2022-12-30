#include "sese/net/ws/WebSocket.h"

sese::net::ws::WebSocket::WebSocket(Socket &socket) noexcept: Socket(socket) {
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
        if (this->readUint16(len) != sizeof(len)) {
            return false;
        }
        info.length = len;
    } else {
        uint64_t len;
        if (this->readUint64(len) != sizeof(len)) {
            return false;
        }
        info.length = len;
    }

    if (info.mask) {
        // todo 验证大小端转换在此处是否是多余的
        if (this->readUint32(info.maskingKey) != sizeof(info.maskingKey)) {
            return false;
        }
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
        if (this->writeUint16(info.length) != sizeof(uint16_t)) {
            return false;
        }
    } else {
        buffer.PAYLOAD_LEN = 127;
        if (this->write(&buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
        if (this->writeUint64(info.length) != sizeof(uint64_t)) {
            return false;
        }
    }

    if (info.mask) {
        // todo 验证大小端转换在此处是否是多余的
        if (this->writeUint32(info.maskingKey) != sizeof(info.maskingKey)) {
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
        temp[i] = ((uint8_t *)buffer)[i] ^ ((const uint8_t *) &maskingKey)[i % 4];
    }
    return this->write(temp, len);
}

bool sese::net::ws::WebSocket::ping() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PING;
    return this->write(&buffer, sizeof(buffer)) != sizeof(buffer);
}

bool sese::net::ws::WebSocket::pong() noexcept {
    struct FrameHeaderBuffer buffer;
    buffer.FIN = 1;
    buffer.OPCODE = SESE_WS_OPCODE_PONG;
    return this->write(&buffer, sizeof(buffer)) != sizeof(buffer);
}

bool sese::net::ws::WebSocket::closeWithError(void *error, size_t length) noexcept {
    struct FrameHeaderInfo info;
    info.fin = true;
    info.opCode = SESE_WS_OPCODE_CLOSE;
    info.length = length;
    return this->writeInfo(info);
}