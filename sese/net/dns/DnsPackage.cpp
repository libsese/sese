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
            break;
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
        answers.push_back(std::move(answer));
    }
    return true;
}


DnsPackage::Ptr DnsPackage::decode(const uint8_t *buffer, size_t length) {
    if (length < 12) {
        return nullptr;
    }

    auto result = new_();
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;

#define DECODE_HEADER(field, offset)    \
    memcpy(&field, buffer + offset, 2); \
    field = FromBigEndian16(field);

    DECODE_HEADER(result->header.id, 0);
    DECODE_HEADER(result->header.flags, 2);
    DECODE_HEADER(qdcount, 4);
    DECODE_HEADER(ancount, 6);
    DECODE_HEADER(nscount, 8);
    DECODE_HEADER(arcount, 10);
#undef DECODE_HEADER

    size_t pos = 12;
    result->questions.reserve(qdcount);
    for (uint16_t i = 0; i < qdcount; i++) {
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

    if (!decodeAnswers(result->answers, ancount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->authorities, nscount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->additionals, arcount, buffer, length, pos)) {
        return nullptr;
    }

    return result;
}

DnsPackage::Index DnsPackage::buildIndex() {
    return {questions, answers, authorities, additionals};
}

bool DnsPackage::encode(void *buffer, size_t &length, Index &index) const {
    index.clearPos();
    const size_t max = length;
    length = 0;
    if (max == 0) {
        length += 12;
    } else {
        if (max - length < 12) {
            return false;
        }
        uint16_t i = ToBigEndian16(header.id);
        memcpy(static_cast<char *>(buffer) + length + 0, &i, 2);
        i = ToBigEndian16(header.flags);
        memcpy(static_cast<char *>(buffer) + length + 2, &i, 2);
        i = ToBigEndian16(static_cast<uint16_t>(questions.size()));
        memcpy(static_cast<char *>(buffer) + length + 4, &i, 2);
        i = ToBigEndian16(static_cast<uint16_t>(answers.size()));
        memcpy(static_cast<char *>(buffer) + length + 6, &i, 2);
        i = ToBigEndian16(static_cast<uint16_t>(authorities.size()));
        memcpy(static_cast<char *>(buffer) + length + 8, &i, 2);
        i = ToBigEndian16(static_cast<uint16_t>(additionals.size()));
        memcpy(static_cast<char *>(buffer) + length + 10, &i, 2);
        length += 12;
    }

    size_t sublength = max == 0 ? max : max - length;
    if (!encodeQuestions(questions, static_cast<char *>(buffer) + length, sublength, index, length)) {
        return false;
    }
    length += sublength;
    sublength = max == 0 ? max : max - length;
    if (!encodeAnswers(answers, static_cast<char *>(buffer) + length, sublength, index, length)) {
        return false;
    }
    length += sublength;
    sublength = max == 0 ? max : max - length;
    if (!encodeAnswers(authorities, static_cast<char *>(buffer) + length, sublength, index, length)) {
        return false;
    }
    length += sublength;
    sublength = max == 0 ? max : max - length;
    if (!encodeAnswers(additionals, static_cast<char *>(buffer) + length, sublength, index, length)) {
        return false;
    }

    return true;
}

std::string DnsPackage::encodeWords(const std::string &fullname) {
    text::StringBuilder builder(128);
    auto words = text::StringBuilder::split(fullname, ".");
    for (const auto &word: words) {
        builder.append(static_cast<char>(word.size()));
        builder.append(word);
    }
    builder.append('\0');
    return builder.toString();
}

bool DnsPackage::encodeQuestions(const std::vector<Question> &questions, void *buffer, size_t &length, Index &index, size_t offset) {
    const size_t max = length;
    length = 0;

    for (const auto &item: questions) {
        auto cache = index.compress_mapping.find(item.name);
        std::string name;
        if (cache != index.compress_mapping.end()) {
            // Hit cache
            name = index.encodeWords(item.name, cache->second, static_cast<uint16_t>(length + offset));
        } else {
            // Write directly without caching
            name = encodeWords(item.name);
        }
        if (max == 0) {
            length += name.size();
        } else {
            if (max - length < name.size()) {
                return false;
            }
            memcpy(static_cast<char *>(buffer) + length, name.data(), name.size());
            length += name.size();
        }
        if (max == 0) {
            length += 4;
        } else {
            if (max - length < 4) {
                return false;
            }
            uint16_t i = ToBigEndian16(item.type);
            memcpy(static_cast<char *>(buffer) + length + 0, &i, 2);
            i = ToBigEndian16(item.class_);
            memcpy(static_cast<char *>(buffer) + length + 2, &i, 2);
            length += 4;
        }
    }

    return true;
}

bool DnsPackage::encodeAnswers(const std::vector<Answer> &answers, void *buffer, size_t &length, Index &index, size_t offset) {
    const size_t max = length;
    length = 0;
    for (const auto &item: answers) {
        auto cache = index.compress_mapping.find(item.name);
        std::string name;
        if (cache != index.compress_mapping.end()) {
            // Hit cache
            name = index.encodeWords(item.name, cache->second, static_cast<uint16_t>(length + offset));
        } else {
            // Write directly without caching
            name = encodeWords(item.name);
        }
        if (max == 0) {
            length += name.size();
        } else {
            if (max - length < name.size()) {
                return false;
            }
            memcpy(static_cast<char *>(buffer) + length, name.data(), name.size());
            length += name.size();
        }

        if (max == 0) {
            length += 10 + item.data_length;
        } else {
            if (max - length < 10 + item.data_length) {
                return false;
            }
            uint16_t i16 = ToBigEndian16(item.type);
            memcpy(static_cast<char *>(buffer) + length + 0, &i16, 2);
            i16 = ToBigEndian16(item.class_);
            memcpy(static_cast<char *>(buffer) + length + 2, &i16, 2);
            uint32_t i32 = ToBigEndian32(item.ttl);
            memcpy(static_cast<char *>(buffer) + length + 4, &i32, 4);
            i16 = ToBigEndian16(item.data_length);
            memcpy(static_cast<char *>(buffer) + length + 8, &i16, 2);
            memcpy(static_cast<char *>(buffer) + length + 10, item.data.get(), item.data_length);
            length += 10 + item.data_length;
        }
    }
    return true;
}

