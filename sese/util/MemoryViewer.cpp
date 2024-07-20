#include "sese/util/MemoryViewer.h"

using namespace sese;

char MemoryViewer::toChar(unsigned char ch, bool is_cap) noexcept {
    if (ch >= 0 && ch <= 9) {
        return static_cast<char>(ch + 48);
    } else {
        if (is_cap) {
            return static_cast<char>(ch + 55);
        } else {
            return static_cast<char>(ch + 87);
        }
    }
}

void MemoryViewer::peer(OutputStream *output, void *position, size_t size, bool is_cap) noexcept {
    auto *p = static_cast<uint8_t *>(position);
    for (auto i = 0; i < size; i++) {
        char word[3]{0, 0, ' '};
        word[0] = toChar(*(p + i + 0) / 0x10, is_cap);
        word[1] = toChar(*(p + i + 0) % 0x10, is_cap);
        if (i + 1 == size) {
            output->write(word, 2);
        } else {
            output->write(word, 3);
        }
    }
}

void MemoryViewer::peer8(OutputStream *output, void *position, bool is_cap) noexcept {
    peer(output, position, 1, is_cap);
}

void MemoryViewer::peer16(OutputStream *output, void *position, bool is_cap) noexcept {
    peer(output, position, 2, is_cap);
}

void MemoryViewer::peer32(OutputStream *output, void *position, bool is_cap) noexcept {
    peer(output, position, 4, is_cap);
}

void MemoryViewer::peer64(OutputStream *output, void *position, bool is_cap) noexcept {
    peer(output, position, 8, is_cap);
}

void MemoryViewer::peer16(OutputStream *output, void *position, EndianType type, bool is_cap) noexcept {
    uint16_t data = *static_cast<uint16_t *>(position);
    if (type == EndianType::BIG) {
        data = ToBigEndian16(data);
    } else {
        data = ToLittleEndian16(data);
    }
    peer(output, &data, 2, is_cap);
}

void MemoryViewer::peer32(OutputStream *output, void *position, EndianType type, bool is_cap) noexcept {
    uint32_t data = *static_cast<uint32_t *>(position);
    if (type == EndianType::BIG) {
        data = ToBigEndian32(data);
    } else {
        data = ToLittleEndian32(data);
    }
    peer(output, &data, 4, is_cap);
}

void MemoryViewer::peer64(OutputStream *output, void *position, EndianType type, bool is_cap) noexcept {
    uint64_t data = *static_cast<uint64_t *>(position);
    if (type == EndianType::BIG) {
        data = ToBigEndian64(data);
    } else {
        data = ToLittleEndian64(data);
    }
    peer(output, &data, 8, is_cap);
}