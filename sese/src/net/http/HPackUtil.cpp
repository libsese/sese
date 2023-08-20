/**
 * HPACK 的键值对处理规则
 *      0.索引的 HEADER 字段
 *          1 + index -> indexedName + indexedValue
 *      1.带索引的字面 HEADER 字段
 *          01 + index -> indexedName + 编码 value
 *          01 + 0     -> 编码 key + 编码 value
 *      2.不带索引的字面 HEADER 字段
 *          0000 + index -> indexedName + 编码 value
 *          0000 + 0     -> 编码 key + 编码 value
 *      3.从不索引的字面 HEADER 字段
 *          0001 + index -> indexedName + 编码 value
 *          0001 + 0     -> 编码 key + 编码 value
 */

#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/HPackUtil.h"
#include "sese/net/http/HPACK.h"
#include "sese/text/DateTimeFormatter.h"

#include <cmath>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

using namespace sese::net::http;

HuffmanDecoder HPackUtil::decoder{};
HuffmanEncoder HPackUtil::encoder{};

bool HPackUtil::decode(InputStream *src, size_t contentLength, DynamicTable &table, Header &header) noexcept {
    uint8_t buf;
    size_t len = 0;
    while (len < contentLength) {
        src->read(&buf, 1);
        len += 1;
        /// 对应第 0 种情况
        if (buf & 0b1000'0000) {
            uint32_t index = 0;
            auto l = decodeInteger(buf, src, index, 7);
            if (-1 == l) return false;
            len += l;
            if (index == 0) return false;

            auto pair = table.get(index);
            if (pair == std::nullopt) return false;
            if (strcasecmp(pair->first.c_str(), "Cookie") == 0) {
                auto cookies = HttpUtil::parseFromCookie(pair->second);
                header.setCookies(cookies);
            } else {
                header.set(pair->first, pair->second);
            }
        } else {
            uint32_t index = 0;
            bool isStore;
            /// 对应第 1 种情况
            if (0b0100'0000 == (buf & 0b1100'0000)) {
                // 添加至动态表
                auto l = decodeInteger(buf, src, index, 6);
                if (-1 == l) return false;
                len += l;
                isStore = true;
            }
            /// 对应第 2 种和第 3 种情况
            else {
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

            if (strcasecmp(key.c_str(), "Cookie") == 0) {
                auto cookies = HttpUtil::parseFromCookie(value);
                header.setCookies(cookies);
            } else {
                header.set(key, value);
            }
        }
    }
    return true;
}

size_t HPackUtil::encode(OutputStream *dest, DynamicTable &table, Header &onceHeader, Header &indexedHeader) noexcept {
    size_t size = 0;
    // 处理索引的 HEADERS
    for (const auto &item: indexedHeader) {
        // 动态表查询
        {
            auto iteratorKey = table.end();
            auto iteratorAll = table.end();
            for (auto header = table.begin(); header != table.end(); ++header) {
                if (header->first == item.first) {
                    iteratorKey = header;
                    if (header->second == item.second) {
                        iteratorAll = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(table.begin(), table.end(), isHitAll);
            if (iteratorAll != table.end()) {
                /// 对应第 0 种情况
                size_t index = table.begin() - iteratorAll + 62;
                size += encodeIndexCase0(dest, index);
                continue;
            }

            // iterator = std::find_if(table.begin(), table.end(), isHit);
            // 存在动态表中
            if (iteratorKey != table.end()) {
                size_t index = table.begin() - iteratorKey + 62;
                /// 对应第 1 种情况
                size += encodeIndexCase1(dest, index);
                size += encodeString(dest, item.second);
                /// 添加动态表
                table.set(item.first, item.second);
                continue;
            }
        }
        // 静态表查询
        {
            auto iteratorKey = predefined_headers.end();
            auto iteratorAll = predefined_headers.end();
            for (auto header = predefined_headers.begin(); header != predefined_headers.end(); ++header) {
                if (header->first == item.first) {
                    iteratorKey = header;
                    if (header->second == item.second) {
                        iteratorAll = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHitAll);
            if (iteratorAll != predefined_headers.end()) {
                /// 对应第 0 种情况
                size_t index = iteratorAll - predefined_headers.begin();
                size += encodeIndexCase0(dest, index);
                continue;
            }

            // iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHit);
            if (iteratorKey != predefined_headers.end()) {
                size_t index = iteratorKey - predefined_headers.begin();
                /// 对应第 1 种情况
                size += encodeIndexCase1(dest, index);
                size += encodeString(dest, item.second);
                /// 添加动态表
                table.set(item.first, item.second);
                continue;
            }
        }

        /// 添加动态表
        {
            size += encodeIndexCase1(dest, 0);
            size += encodeString(dest, item.first);
            size += encodeString(dest, item.second);
            table.set(item.first, item.second);
            continue;
        }
    }

    // 处理一次性 HEADERS
    for (const auto &item: onceHeader) {
        // 动态表查询
        {
            auto iteratorKey = table.end();
            auto iteratorAll = table.end();
            for (auto header = table.begin(); header != table.end(); ++header) {
                if (header->first == item.first) {
                    iteratorKey = header;
                    if (header->second == item.second) {
                        iteratorAll = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(table.begin(), table.end(), isHit);
            if (iteratorAll != table.end()) {
                /// 第 0 种情况
                size += encodeIndexCase0(dest, iteratorAll - table.begin() + 62);
                continue;
            }

            if (iteratorKey != table.end()) {
                /// 第 2 种情况
                size += encodeIndexCase2(dest, iteratorKey - table.begin() + 62);
                size += encodeString(dest, item.second);
                continue;
            }
        }

        // 静态表查询
        {
            auto iteratorKey = predefined_headers.end();
            auto iteratorAll = predefined_headers.end();
            for (auto header = predefined_headers.begin(); header != predefined_headers.end(); ++header) {
                if (header->first == item.first) {
                    iteratorKey = header;
                    if (header->second == item.second) {
                        iteratorAll = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHit);
            if (iteratorAll != predefined_headers.end()) {
                /// 第 0 种情况
                size += encodeIndexCase0(dest, iteratorAll - predefined_headers.begin());
                continue;
            }
            if (iteratorKey != predefined_headers.end()) {
                /// 第 2 种情况
                size += encodeIndexCase2(dest, iteratorKey - predefined_headers.begin());
                size += encodeString(dest, item.second);
                continue;
            }
        }

        // 无任何索引数据
        {
            size += encodeIndexCase3(dest, 0);
            size += encodeString(dest, item.first);
            size += encodeString(dest, item.second);
            continue;
        }
    }

    /// 此处未对 Cookies 进行压缩
    auto onceCookies = onceHeader.getCookies();
    auto indexedCookies = indexedHeader.getCookies();
    if (onceCookies) {
        for (const auto &cookie: *onceCookies) {
            auto str = buildCookieString(cookie.second);
            size += encodeIndexCase2(dest, 0);
            size += encodeString(dest, "Set-Cookie");
            size += encodeString(dest, str);
        }
    }
    if (indexedCookies) {
        for (const auto &cookie: *indexedCookies) {
            auto str = buildCookieString(cookie.second);
            size += encodeIndexCase2(dest, 0);
            size += encodeString(dest, "Set-Cookie");
            size += encodeString(dest, str);
        }
    }

    return size;
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

size_t HPackUtil::encodeIndexCase0(OutputStream *dest, size_t index) noexcept {
    const auto prefix = static_cast<uint8_t>(std::pow(2, 7) - 1);
    uint8_t buf;
    if (index < prefix) {
        buf = 0b1000'0000 | (((uint8_t) index) & 0b0111'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b1000'0000 | 0b0111'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= prefix;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        return size;
    }
}

size_t HPackUtil::encodeIndexCase1(OutputStream *dest, size_t index) noexcept {
    const auto prefix = static_cast<uint8_t>(std::pow(2, 6) - 1);
    uint8_t buf;
    if (index < prefix) {
        buf = 0b0100'0000 | (((uint8_t) index) & 0b0011'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0100'0000 | 0b0011'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= prefix;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = (uint8_t) index;
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeIndexCase2(OutputStream *dest, size_t index) noexcept {
    const auto prefix = static_cast<uint8_t>(std::pow(2, 4) - 1);
    uint8_t buf;
    if (index < prefix) {
        buf = 0b0000'0000 | (((uint8_t) index) & 0b0000'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0000'0000 | 0b0000'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= prefix;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = (uint8_t) index;
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeIndexCase3(sese::OutputStream *dest, size_t index) noexcept {
    const auto prefix = static_cast<uint8_t>(std::pow(2, 4) - 1);
    uint8_t buf;
    if (index < prefix) {
        buf = 0b0001'0000 | (((uint8_t) index) & 0b0000'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0001'0000 | 0b0000'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= prefix;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = (uint8_t) index;
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeString(OutputStream *dest, const std::string &str) noexcept {
    const auto prefix = static_cast<uint8_t>(std::pow(2, 7) - 1);
    auto strLen = str.length();
    if (strLen > 8) {
        /// 需要 Huffman 压缩
        uint8_t buf;
        auto code = encoder.encode(str);
        if (code.size() < prefix) {
            buf = 0b1000'0000 | ((uint8_t) code.size());
            dest->write(&buf, 1);
            dest->write(code.data(), code.size());
            return 1 + code.size();
        } else {
            buf = 0b1000'0000 | 0b0111'1111;
            dest->write(&buf, 1);
            size_t size = 1;
            size_t i = code.size();
            i -= prefix;
            while (i >= 128) {
                buf = i % 128 + 128;
                dest->write(&buf, 1);
                i = i / 128;
                size += 1;
            }
            buf = (uint8_t) i;
            dest->write(&buf, 1);
            size += 1;
            dest->write(code.data(), code.size());
            return size + code.size();
        }
    } else {
        /// 不需要 Huffman 压缩
        uint8_t buf;
        if (str.size() < prefix) {
            buf = 0b0000'0000 | ((uint8_t) str.size());
            dest->write(&buf, 1);
            dest->write(str.data(), str.size());
            return 1 + str.size();
        } else {
            buf = 0b0000'0000 | 0b0111'1111;
            dest->write(&buf, 1);
            size_t size = 1;
            size_t i = str.size();
            i -= prefix;
            while (i >= 128) {
                buf = i % 128 + 128;
                dest->write(&buf, 1);
                i = i / 128;
                size += 1;
            }
            buf = (uint8_t) i;
            dest->write(&buf, 1);
            size += 1;
            dest->write(str.data(), str.size());
            return size + str.size();
        }
    }
}

std::string HPackUtil::buildCookieString(const Cookie::Ptr &cookie) noexcept {
    std::stringstream stream;
    const std::string &name = cookie->getName();
    const std::string &value = cookie->getValue();
    stream << name << "=" << value;

    const std::string &path = cookie->getPath();
    if (!path.empty()) {
        stream << "; " << path;
    }

    const std::string &domain = cookie->getDomain();
    if (!domain.empty()) {
        stream << "; " << domain;
    }

    uint64_t maxAge = cookie->getMaxAge();
    if (maxAge > 0) {
        stream << "; Max-Age=" << maxAge;
    } else {
        uint64_t expires = cookie->getExpires();
        if (expires > 0) {
            auto date = DateTime(expires, 0);
            auto dateString = sese::text::DateTimeFormatter::format(date, TIME_GREENWICH_MEAN_PATTERN);
            stream << "; Expires=" << dateString;
        }
    }

    bool secure = cookie->isSecure();
    if (secure) {
        stream << "; Secure";
    }

    bool httpOnly = cookie->isHttpOnly();
    if (httpOnly) {
        stream << "; HttpOnly";
    }

    return stream.str();
}