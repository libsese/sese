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

#include <iterator>
#include <sese/util/Endian.h>
#include <sese/text/StringBuilder.h>

#include <map>

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

#define DECODE_HEADER(field, offset)    \
    memcpy(&field, buffer + offset, 2); \
    field = FromBigEndian16(field);

    DECODE_HEADER(result->header.id, 0);
    DECODE_HEADER(result->header.flags, 2);
    DECODE_HEADER(result->header.qdcount, 4);
    DECODE_HEADER(result->header.ancount, 6);
    DECODE_HEADER(result->header.nscount, 8);
    DECODE_HEADER(result->header.arcount, 10);
#undef DECODE_HEADER

    size_t pos = 12;
    result->questions.reserve(result->header.qdcount);
    for (uint16_t i = 0; i < result->header.qdcount; i++) {
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

    if (!decodeAnswers(result->answers, result->header.ancount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->authorities, result->header.nscount, buffer, length, pos)) {
        return nullptr;
    }
    if (!decodeAnswers(result->additionals, result->header.arcount, buffer, length, pos)) {
        return nullptr;
    }

    return result;
}

DnsPackage::Index DnsPackage::buildIndex() {
    Index domain_index;

    // 建立索引
    std::map<std::string, Index::CompressIndex> compress_string_index;
    uint16_t index = 0;
#define BUILD_INDEX(object_list)                                                                           \
    for (auto &&item: object_list) {                                                                       \
        auto names = text::StringBuilder::split(item.name, ".");                                           \
        index += static_cast<uint16_t>(names.size());                                                      \
        std::string name;                                                                                  \
        bool first = true;                                                                                 \
        auto pos = 0;                                                                                      \
        auto iterator0 = domain_index.compress_mapping.find(item.name);                                    \
        if (iterator0 == domain_index.compress_mapping.end()) {                                            \
            iterator0 = domain_index.compress_mapping.insert({item.name, Index::CompressMapping{}}).first; \
        }                                                                                                  \
        for (auto i = static_cast<uint16_t>(names.size()); i > 0; i--) {                                   \
            if (!first)                                                                                    \
                name = "." + name;                                                                         \
            name = names[i - 1] + name;                                                                    \
            auto iterator1 = compress_string_index.find(name);                                             \
            if (iterator1 == compress_string_index.end()) {                                                \
                auto tmp = static_cast<uint16_t>(index - pos);                                             \
                pos++;                                                                                     \
                compress_string_index[name] = {tmp, name, 0};                                              \
                iterator0->second.emplace(tmp);                                                            \
            } else {                                                                                       \
                iterator0->second.emplace(iterator1->second.index);                                        \
            }                                                                                              \
            first = false;                                                                                 \
        }                                                                                                  \
    }

    BUILD_INDEX(questions);
    BUILD_INDEX(answers);
    BUILD_INDEX(authorities);
    BUILD_INDEX(additionals);
#undef BUILD_INDEX
    // 索引换键
    domain_index.compress_index.reserve(compress_string_index.size());
    std::transform(
        compress_string_index.begin(),
        compress_string_index.end(),
        std::back_inserter(domain_index.compress_index),
        [](const std::pair<const std::string, Index::CompressIndex> &item) {
            return item.second;
        }
    );
    compress_string_index.clear();

    return domain_index;
}