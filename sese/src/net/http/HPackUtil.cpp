#include "sese/net/http/HPackUtil.h"
#include "sese/net/http/Huffman.h"

#include <cmath>

using namespace sese::net::http;

bool HPackUtil::decode(InputStream *src, size_t contentLength, DynamicTable &table, Header &header) noexcept {
    uint8_t buf;
    int64_t len = 0;
    while (len < contentLength) {
        src->read(&buf, 1);
        len += 1;
        // key & value 均在索引
        if (buf & 0b1000'0000) {
            uint32_t index = 0;
            auto l = decodeInteger(buf, src, index, 7);
            if (-1 == l) return false;
            len += l;
            if (index == 0) return false;

            auto pair = table.get(index);
            if (pair == std::nullopt) return false;
            header.set(pair->first, pair->second);
        } else {
            uint32_t index = 0;
            bool isStore;
            if (0b0100'0000 == (buf & 0b1100'0000)) {
                // 添加至动态表
                auto l = decodeInteger(buf, src, index, 6);
                if (-1 == l) return false;
                len += l;
                isStore = true;
            } else {
                // 不添加至动态表
                auto l = decodeInteger(buf, src, index, 4);
                if (-1 == l) return false;
                len += l;
                isStore = false;
            }

            std::string key;
            if (0 != index) {
                auto ret = table.get(index);
                if (ret == std::nullopt) {
                    return false;
                }
                key = ret.value().first;
            } else {
                auto l = decodeString(src, key);
                if (-1 == l) {
                    return false;
                } else {
                    len += l;
                }
            }

            std::string value;
            auto l = decodeString(src, value);
            if (-1 == l) {
                return false;
            } else {
                len += l;
            }

            if (isStore) {
                table.set(key, value);
            }

            header.set(key, value);
        }
    }
    return true;
}

int HPackUtil::decodeInteger(uint8_t &buf, InputStream *src, uint32_t &dest, uint8_t n) noexcept {
    const auto two_N = static_cast<uint16_t>(std::pow(2, n) - 1);
    dest = buf & two_N;
    if (dest == two_N) {
        uint64_t M = 0;
        int len = 0;
        while ((src->read(&buf, 1)) > 0) {
            dest += (buf & 0x7F) << M;
            M += 7;
            len += 1;

            if (!(buf & 0x80)) {
                return len;
            }
        }
        return -1;
    } else {
        return 0;
    }
}

int HPackUtil::decodeString(InputStream *src, std::string &dest) noexcept {
    uint8_t buf;
    src->read(&buf, 1);
    uint8_t len = (buf & 0x7F);
    bool isHuffman = (buf & 0x80) == 0x80;

    char buffer[UINT8_MAX]{};
    if (len != src->read(buffer, len)) {
        return -1;
    }

    if (isHuffman) {
        auto result = decoder.decode(buffer, len);
        if (result != std::nullopt) {
            dest = decoder.decode(buffer, len).value();
        }
    } else {
        dest = buffer;
    }
    return len + 1;
}