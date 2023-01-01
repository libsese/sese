#include "MemoryViewer.h"

using namespace sese;

char MemoryViewer::toChar(unsigned char ch, bool isCap) noexcept {
    if (ch >= 0 && ch <= 9) {
        return (char) (ch + 48);
    } else {
        if (isCap) {
            return (char) (ch + 55);
        } else {
            return (char) (ch + 87);
        }
    }
}

void MemoryViewer::peer(const OutputStream::Ptr &output, void *position, size_t size, bool isCap) noexcept {
    auto *p = (uint8_t *) position;
    for (auto i = 0; i < size; i++) {
        char word[3]{0, 0, ' '};
        word[0] = toChar(*(p + i + 0) / 0x10, isCap);
        word[1] = toChar(*(p + i + 0) % 0x10, isCap);
        if (i + 1 == size) {
            output->write(word, 2);
        } else {
            output->write(word, 3);
        }
    }
}

void MemoryViewer::peer8(const OutputStream::Ptr &output, void *position, bool isCap) noexcept {
    peer(output, position, 1, isCap);
}

void MemoryViewer::peer16(const OutputStream::Ptr &output, void *position, bool isCap) noexcept {
    peer(output, position, 2, isCap);
}

void MemoryViewer::peer32(const OutputStream::Ptr &output, void *position, bool isCap) noexcept {
    peer(output, position, 4, isCap);
}

void MemoryViewer::peer64(const OutputStream::Ptr &output, void *position, bool isCap) noexcept {
    peer(output, position, 8, isCap);
}

void MemoryViewer::peer16(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap) noexcept {
    uint16_t data = *(uint16_t *)position;
    if (type == EndianType::Big) {
        data = ToBigEndian16(data);
    } else {
        data = ToLittleEndian16(data);
    }
    peer(output, &data, 2, isCap);
}

void MemoryViewer::peer32(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap) noexcept {
    uint32_t data = *(uint32_t *)position;
    if (type == EndianType::Big) {
        data = ToBigEndian32(data);
    } else {
        data = ToLittleEndian32(data);
    }
    peer(output, &data, 4, isCap);
}

void MemoryViewer::peer64(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap) noexcept {
    uint64_t data = *(uint64_t *)position;
    if (type == EndianType::Big) {
        data = ToBigEndian64(data);
    } else {
        data = ToLittleEndian64(data);
    }
    peer(output, &data, 8, isCap);
}