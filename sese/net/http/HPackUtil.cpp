// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * HPACK Key-Value Pair Processing Rules
 *      0. Indexed HEADER Field
 *          1 + index -> indexedName + indexedValue
 *      1. Literal HEADER Field with Incremental Indexing
 *          01 + index -> indexedName + Encoded value
 *          01 + 0     -> Encoded key + Encoded value
 *      2. Literal HEADER Field without Indexing
 *          0000 + index -> indexedName + Encoded value
 *          0000 + 0     -> Encoded key + Encoded value
 *      3. Literal HEADER Field never Indexed
 *          0001 + index -> indexedName + Encoded value
 *          0001 + 0     -> Encoded key + Encoded value
 *      4. Update Dynamic Table Size
 *          001 + size
 */

#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/HPackUtil.h"
#include "sese/net/http/HPACK.h"
#include "sese/net/http/Http2Frame.h"
#include "sese/text/DateTimeFormatter.h"
#include "sese/util/DateTime.h"
#include "sese/text/StringBuilder.h"

#include <cmath>
#include <algorithm>

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

using namespace sese::net::http;

const std::string HPackUtil::REQ_PSEUDO_HEADER[4]{":method", ":scheme", ":authority", ":path"};

HuffmanDecoder HPackUtil::decoder{};
HuffmanEncoder HPackUtil::encoder{};

bool HPackUtil::setHeader(Header &header, const std::string &key, const std::string &value) noexcept {
    auto iter = header.find(key);
    if (iter == header.end()) {
        header.set(key, value);
        return true;
    }

    if (key == ":status") {
        return false;
    }
    if (std::find(std::begin(REQ_PSEUDO_HEADER), std::end(REQ_PSEUDO_HEADER), key) != std::end(REQ_PSEUDO_HEADER)) {
        return false;
    }

    header.set(key, value);
    return true;
}

bool HPackUtil::verifyHeader(Header &header, bool is_resp) noexcept {
    if (is_resp) {
        for (auto &&item: REQ_PSEUDO_HEADER) {
            if (header.exist(item)) {
                return false;
            }
        }
        if (!header.exist(":status")) {
            return false;
        }
    } else {
        for (auto &&item: REQ_PSEUDO_HEADER) {
            if (!header.exist(item)) {
                return false;
            }
        }
        if (header.exist(":status")) {
            return false;
        }
    }
    return true;
}

uint32_t HPackUtil::decode(
    InputStream *src,
    size_t content_length,
    DynamicTable &table,
    Header &header,
    bool is_resp,
    bool has_pseudo,
    uint32_t limit
) noexcept {
    uint8_t buf;
    size_t len = 0;
    while (len < content_length) {
        if (1 != src->read(&buf, 1)) {
            return GOAWAY_COMPRESSION_ERROR;
        }
        len += 1;
        /// Corresponds to case 0
        if (buf & 0b1000'0000) {
            uint32_t index = 0;
            auto l = decodeInteger(buf, src, index, 7);
            if (-1 == l) {
                return GOAWAY_COMPRESSION_ERROR;
            }
            len += l;
            if (index == 0) {
                return GOAWAY_COMPRESSION_ERROR;
            }

            auto pair = table.get(index);
            if (pair == std::nullopt) {
                return GOAWAY_COMPRESSION_ERROR;
            }
            if (strcmpDoNotCase(pair->first.c_str(), "Cookie")) {
                auto cookies = HttpUtil::parseFromCookie(pair->second);
                header.setCookies(cookies);
            } else {
                if (*pair->first.begin() == ':') {
                    if (!has_pseudo) {
                        return GOAWAY_COMPRESSION_ERROR;
                    }
                    if (!setHeader(header, pair->first, pair->second)) {
                        return GOAWAY_PROTOCOL_ERROR;
                    }
                } else {
                    has_pseudo = false;
                    header.set(pair->first, pair->second);
                }
            }
        }
        // Corresponding to the fourth case
        else if ((buf & 0b1110'0000) == 0b0010'0000) {
            uint32_t new_size;
            auto l = decodeInteger(buf, src, new_size, 5);
            if (-1 == l) {
                return GOAWAY_COMPRESSION_ERROR;
            }
            len += l;
            if (new_size > limit) {
                return GOAWAY_COMPRESSION_ERROR;
            }
            table.resize(new_size);
        } else {
            uint32_t index = 0;
            bool is_store;
            /// Corresponding to case 1
            if ((buf & 0b1100'0000) == 0b0100'0000) {
                // Add to a dynamic table
                auto l = decodeInteger(buf, src, index, 6);
                if (-1 == l) {
                    return GOAWAY_COMPRESSION_ERROR;
                }
                len += l;
                is_store = true;
            }
            /// Corresponding to the 2nd and 3rd cases
            else {
                // Not added to a dynamic table
                auto l = decodeInteger(buf, src, index, 4);
                if (-1 == l) {
                    return GOAWAY_COMPRESSION_ERROR;
                }
                len += l;
                is_store = false;
            }

            std::string key;
            if (0 != index) {
                auto ret = table.get(index);
                if (ret == std::nullopt) {
                    return GOAWAY_COMPRESSION_ERROR;
                }
                key = ret.value().first;
            } else {
                auto l = decodeString(src, key);
                if (-1 == l) {
                    return GOAWAY_COMPRESSION_ERROR;
                }
                len += l;
            }

            std::string value;
            auto l = decodeString(src, value);
            if (-1 == l) {
                return GOAWAY_COMPRESSION_ERROR;
            }
            len += l;

            if (is_store) {
                table.set(key, value);
            }

            if (strcmpDoNotCase(key.c_str(), "Cookie")) {
                auto cookies = HttpUtil::parseFromCookie(value);
                header.setCookies(cookies);
            } else {
                if (*key.begin() == ':') {
                    if (!has_pseudo) {
                        return GOAWAY_COMPRESSION_ERROR;
                    }
                    if (!setHeader(header, key, value)) {
                        return GOAWAY_PROTOCOL_ERROR;
                    }
                } else {
                    has_pseudo = false;
                    header.set(key, value);
                }
            }
        }
    }

    if (!verifyHeader(header, is_resp)) {
        return GOAWAY_PROTOCOL_ERROR;
    }
    return 0;
}

size_t HPackUtil::encode(OutputStream *dest, DynamicTable &table, Header &once_header,
                         Header &indexed_header) noexcept {
    size_t size = 0;
    // HANDLES INDEXED HEADERS
    for (const auto &item: indexed_header) {
        // Dynamic table queries
        {
            auto iterator_key = table.end();
            auto iterator_all = table.end();
            for (auto header = table.begin(); header != table.end(); ++header) {
                if (header->first == item.first) {
                    iterator_key = header;
                    if (header->second == item.second) {
                        iterator_all = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(table.begin(), table.end(), isHitAll);
            if (iterator_all != table.end()) {
                /// Corresponds to case 0
                size_t index = table.getCount() - 1 - (iterator_all - table.begin()) + PREDEFINED_HEADERS.size();
                size += encodeIndexCase0(dest, index);
                continue;
            }

            // iterator = std::find_if(table.begin(), table.end(), isHit);
            // Exists in a dynamic table
            if (iterator_key != table.end()) {
                size_t index = table.getCount() - 1 - (iterator_key - table.begin()) + PREDEFINED_HEADERS.size();
                /// Corresponding to case 1
                size += encodeIndexCase1(dest, index);
                size += encodeString(dest, item.second);
                /// Add a dynamic table
                table.set(item.first, item.second);
                continue;
            }
        }
        // Static table queries
        {
            auto iterator_key = PREDEFINED_HEADERS.end();
            auto iterator_all = PREDEFINED_HEADERS.end();
            for (auto header = PREDEFINED_HEADERS.begin(); header != PREDEFINED_HEADERS.end(); ++header) {
                if (header->first == item.first) {
                    iterator_key = header;
                    if (header->second == item.second) {
                        iterator_all = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHitAll);
            if (iterator_all != PREDEFINED_HEADERS.end()) {
                /// Corresponds to case 0
                size_t index = iterator_all - PREDEFINED_HEADERS.begin();
                size += encodeIndexCase0(dest, index);
                continue;
            }

            // iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHit);
            if (iterator_key != PREDEFINED_HEADERS.end()) {
                size_t index = iterator_key - PREDEFINED_HEADERS.begin();
                /// Corresponding to case 1
                size += encodeIndexCase1(dest, index);
                size += encodeString(dest, item.second);
                /// Add a dynamic table
                table.set(item.first, item.second);
                continue;
            }
        }

        /// Add a dynamic table
        {
            size += encodeIndexCase1(dest, 0);
            size += encodeString(dest, item.first);
            size += encodeString(dest, item.second);
            table.set(item.first, item.second);
            continue;
        }
    }

    // DISPOSE OF SINGLE-USE HEADERS
    for (const auto &item: once_header) {
        // Dynamic table queries
        {
            auto iterator_key = table.end();
            auto iterator_all = table.end();
            for (auto header = table.begin(); header != table.end(); ++header) {
                if (header->first == item.first) {
                    iterator_key = header;
                    if (header->second == item.second) {
                        iterator_all = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(table.begin(), table.end(), isHit);
            if (iterator_all != table.end()) {
                /// Case 0
                size += encodeIndexCase0(dest, iterator_all - table.begin() + 62);
                continue;
            }

            if (iterator_key != table.end()) {
                /// Case 2
                size += encodeIndexCase2(dest, iterator_key - table.begin() + 62);
                size += encodeString(dest, item.second);
                continue;
            }
        }

        // Static table queries
        {
            auto iterator_key = PREDEFINED_HEADERS.end();
            auto iterator_all = PREDEFINED_HEADERS.end();
            for (auto header = PREDEFINED_HEADERS.begin(); header != PREDEFINED_HEADERS.end(); ++header) {
                if (header->first == item.first) {
                    iterator_key = header;
                    if (header->second == item.second) {
                        iterator_all = header;
                        break;
                    }
                }
            }

            // auto iterator = std::find_if(predefined_headers.begin(), predefined_headers.end(), isHit);
            if (iterator_all != PREDEFINED_HEADERS.end()) {
                /// Case 0
                size += encodeIndexCase0(dest, iterator_all - PREDEFINED_HEADERS.begin());
                continue;
            }
            if (iterator_key != PREDEFINED_HEADERS.end()) {
                /// Case 2
                size += encodeIndexCase2(dest, iterator_key - PREDEFINED_HEADERS.begin());
                size += encodeString(dest, item.second);
                continue;
            }
        }

        // There is no indexed data
        {
            size += encodeIndexCase3(dest, 0);
            size += encodeString(dest, item.first);
            size += encodeString(dest, item.second);
            continue;
        }
    }

    /// Cookies are not compressed here
    auto once_cookies = once_header.getCookies();
    auto indexed_cookies = indexed_header.getCookies();
    if (once_cookies) {
        for (const auto &cookie: *once_cookies) {
            auto str = buildCookieString(cookie.second);
            size += encodeIndexCase2(dest, 0);
            size += encodeString(dest, "Set-Cookie");
            size += encodeString(dest, str);
        }
    }
    if (indexed_cookies) {
        for (const auto &cookie: *indexed_cookies) {
            auto str = buildCookieString(cookie.second);
            size += encodeIndexCase2(dest, 0);
            size += encodeString(dest, "Set-Cookie");
            size += encodeString(dest, str);
        }
    }

    return size;
}

int HPackUtil::decodeInteger(uint8_t &buf, InputStream *src, uint32_t &dest, uint8_t n) noexcept {
    const auto TWO_N = static_cast<uint16_t>(std::pow(2, n) - 1);
    dest = buf & TWO_N;
    if (dest == TWO_N) {
        uint64_t m = 0;
        int len = 0;
        while ((src->read(&buf, 1)) > 0) {
            dest += (buf & 0x7F) << m;
            m += 7;
            len += 1;

            if (!(buf & 0x80)) {
                return len;
            }
        }
        return -1;
    }
    return 0;
}

int HPackUtil::decodeString(InputStream *src, std::string &dest) noexcept {
    uint8_t buf;
    src->read(&buf, 1);
    bool is_huffman = (buf & 0x80) == 0x80;

    uint32_t len;
    auto l = decodeInteger(buf, src, len, 7);
    if (l == -1) {
        return -1;
    }
    if (len > UINT16_MAX) {
        return -1;
    }
    char buffer[UINT16_MAX]{};
    if (len != src->read(buffer, len)) {
        return -1;
    }

    if (is_huffman) {
        auto result = decoder.decode(buffer, len);
        if (result == std::nullopt) {
            return -1;
        }
        dest = result.value();
    } else {
        dest = buffer;
    }
    return static_cast<int>(1 + l + len);
}

size_t HPackUtil::encodeIndexCase0(OutputStream *dest, size_t index) noexcept {
    const auto PREFIX = static_cast<uint8_t>(std::pow(2, 7) - 1);
    uint8_t buf;
    if (index < PREFIX) {
        buf = 0b1000'0000 | (static_cast<uint8_t>(index) & 0b0111'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b1000'0000 | 0b0111'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= PREFIX;
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
    const auto PREFIX = static_cast<uint8_t>(std::pow(2, 6) - 1);
    uint8_t buf;
    if (index < PREFIX) {
        buf = 0b0100'0000 | (static_cast<uint8_t>(index) & 0b0011'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0100'0000 | 0b0011'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= PREFIX;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = static_cast<uint8_t>(index);
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeIndexCase2(OutputStream *dest, size_t index) noexcept {
    const auto PREFIX = static_cast<uint8_t>(std::pow(2, 4) - 1);
    uint8_t buf;
    if (index < PREFIX) {
        buf = 0b0000'0000 | (static_cast<uint8_t>(index) & 0b0000'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0000'0000 | 0b0000'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= PREFIX;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = static_cast<uint8_t>(index);
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeIndexCase3(OutputStream *dest, size_t index) noexcept {
    const auto PREFIX = static_cast<uint8_t>(std::pow(2, 4) - 1);
    uint8_t buf;
    if (index < PREFIX) {
        buf = 0b0001'0000 | (static_cast<uint8_t>(index) & 0b0000'1111);
        dest->write(&buf, 1);
        return 1;
    } else {
        buf = 0b0001'0000 | 0b0000'1111;
        dest->write(&buf, 1);
        size_t size = 1;
        index -= PREFIX;
        while (index >= 128) {
            buf = index % 128 + 128;
            dest->write(&buf, 1);
            index = index / 128;
            size += 1;
        }
        buf = static_cast<uint8_t>(index);
        dest->write(&buf, 1);
        size += 1;
        return size;
    }
}

size_t HPackUtil::encodeString(OutputStream *dest, const std::string &str) noexcept {
    const auto PREFIX = static_cast<uint8_t>(std::pow(2, 7) - 1);
    auto str_len = str.length();
    if (str_len > 8) {
        /// Huffman compression is required
        uint8_t buf;
        auto code = encoder.encode(str);
        if (code.size() < PREFIX) {
            buf = 0b1000'0000 | static_cast<uint8_t>(code.size());
            dest->write(&buf, 1);
            dest->write(code.data(), code.size());
            return 1 + code.size();
        } else {
            buf = 0b1000'0000 | 0b0111'1111;
            dest->write(&buf, 1);
            size_t size = 1;
            size_t i = code.size();
            i -= PREFIX;
            while (i >= 128) {
                buf = i % 128 + 128;
                dest->write(&buf, 1);
                i = i / 128;
                size += 1;
            }
            buf = static_cast<uint8_t>(i);
            dest->write(&buf, 1);
            size += 1;
            dest->write(code.data(), code.size());
            return size + code.size();
        }
    } else {
        /// Huffman compression is not required
        uint8_t buf;
        if (str.size() < PREFIX) {
            buf = 0b0000'0000 | static_cast<uint8_t>(str.size());
            dest->write(&buf, 1);
            dest->write(str.data(), str.size());
            return 1 + str.size();
        } else {
            buf = 0b0000'0000 | 0b0111'1111;
            dest->write(&buf, 1);
            size_t size = 1;
            size_t i = str.size();
            i -= PREFIX;
            while (i >= 128) {
                buf = i % 128 + 128;
                dest->write(&buf, 1);
                i = i / 128;
                size += 1;
            }
            buf = static_cast<uint8_t>(i);
            dest->write(&buf, 1);
            size += 1;
            dest->write(str.data(), str.size());
            return size + str.size();
        }
    }
}

std::string HPackUtil::buildCookieString(const Cookie::Ptr &cookie) noexcept {
    text::StringBuilder stream;
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

    uint64_t max_age = cookie->getMaxAge();
    if (max_age > 0) {
        stream << "; Max-Age=" << std::to_string(max_age);
    } else {
        uint64_t expires = cookie->getExpires();
        if (expires > 0) {
            auto date = DateTime(expires, 0);
            auto date_string = sese::text::DateTimeFormatter::format(date, TIME_GREENWICH_MEAN_PATTERN);
            stream << "; Expires=" << date_string;
        }
    }

    if (cookie->isSecure()) {
        stream << "; Secure";
    }

    if (cookie->isHttpOnly()) {
        stream << "; HttpOnly";
    }

    return stream.toString();
}
