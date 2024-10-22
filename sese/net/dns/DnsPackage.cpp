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

#include "DnsPackage.h"

#include <sese/util/Endian.h>
#include <sese/text/StringBuilder.h>

using sese::net::dns::DnsPackage;

DnsPackage::Ptr DnsPackage::new_() {
    return Ptr(new DnsPackage());
}

std::string DnsPackage::decodeWords(const uint8_t *buffer, size_t length, size_t &offset) {
    text::StringBuilder builder(128);
    const uint8_t *current = buffer + offset;
    uint16_t current_pos = 0;
    bool first = true;

    while (current[current_pos] != 0) {
        if ((current[current_pos] & 0xC0) == 0xC0) {
            uint16_t new_offset_ = *reinterpret_cast<const uint16_t *>(current + current_pos);
            new_offset_ = FromBigEndian16(new_offset_);
            new_offset_ &= 0x3FFF;
            size_t new_offset = new_offset_;
            if (new_offset > length) {
                return {};
            }
            auto sub = decodeWords(buffer, length, new_offset);
            if (sub.empty()) {
                return {};
            }
            if (!first) {
                builder.append('.');
            }
            builder.append(sub);
            current_pos += 2;
        } else {
            uint8_t sub_length = current[current_pos];
            if (offset + sub_length + 1 > length) {
                return {};
            }
            if (!first) {
                builder.append('.');
            }
            builder.append(
                reinterpret_cast<const char *>(current + current_pos + 1),
                sub_length
            );
            current_pos += sub_length + 1;
        }
        first = false;
    }
    offset = offset + current_pos;
    return builder.toString();
}

bool DnsPackage::decodeAnswers(std::vector<Answer> &answers, size_t expect_size, const uint8_t *buffer, size_t length, size_t &pos) {
    answers.reserve(expect_size);
    for (size_t i = 0; i < expect_size; i++) {
        std::string name = decodeWords(buffer, length, pos);
        if (name.empty()) {
            return false;
        }
        Answer answer;
        answer.name = name;

        if (pos + 10 > length) {
            return false;
        }
        memcpy(&answer.type, buffer + pos, 2);
        answer.type = FromBigEndian16(answer.type);
        pos += 2;
        memcpy(&answer.class_, buffer + pos, 2);
        answer.class_ = FromBigEndian16(answer.class_);
        pos += 2;
        memcpy(&answer.ttl, buffer + pos, 4);
        answer.ttl = FromBigEndian32(answer.ttl);
        pos += 4;
        memcpy(&answer.data_length, buffer + pos, 2);
        answer.data_length = FromBigEndian16(answer.data_length);
        pos += 2;
        if (pos + answer.data_length > length) {
            return false;
        }
        answer.data = std::make_unique<uint8_t[]>(answer.data_length);
        memcpy(answer.data.get(), buffer + pos, answer.data_length);
        pos += answer.data_length;
    }
    return true;
}


DnsPackage::Ptr DnsPackage::decode(const uint8_t *buffer, size_t length) {
    if (length < 12) {
        return nullptr;
    }

    auto result = new_();

    DnsHeader header{};

#define DECODE_HEADER(field, offset)    \
    memcpy(&field, buffer + offset, 2); \
    field = FromBigEndian16(field);

    DECODE_HEADER(header.id, 0);
    DECODE_HEADER(header.flags, 2);
    DECODE_HEADER(header.qdcount, 4);
    DECODE_HEADER(header.ancount, 6);
    DECODE_HEADER(header.nscount, 8);
    DECODE_HEADER(header.arcount, 10);
#undef DECODE_HEADER

    size_t pos = 12;
    result->questions.reserve(header.qdcount);
    for (uint16_t i = 0; i < header.qdcount; i++) {
        std::string name = decodeWords(buffer, length, pos);
        if (name.empty()) {
            return nullptr;
        }
        Question question;
        question.name = name;
        memcpy(&question.type, buffer + pos + 1, 2);
        question.type = FromBigEndian16(question.type);
        memcpy(&question.class_, buffer + pos + 3, 2);
        question.class_ = FromBigEndian16(question.class_);
        result->questions.emplace_back(question);
        pos += 5;
    }

    if (!decodeAnswers(result->answers, header.ancount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->authorities, header.nscount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->additionals, header.arcount, buffer, length, pos)) {
        return nullptr;
    }

    return result;
}
