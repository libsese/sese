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

#include <sese/text/StringBuilder.h>
#include <sese/util/Endian.h>

#include <map>
#include <iterator>

using sese::net::dns::DnsPackage;

DnsPackage::Index::Index(std::vector<Question> &questions, std::vector<Answer> &answers, std::vector<Authority> &authorities, std::vector<Additional> &additionals) {
    // 建立索引
    std::map<std::string, CompressIndex> compress_string_index;
    uint16_t index = 0;
#define BUILD_INDEX(object_list)                                                              \
    for (auto &&item: object_list) {                                                          \
        auto names = text::StringBuilder::split(item.name, ".");                              \
        index += static_cast<uint16_t>(names.size());                                         \
        std::string name;                                                                     \
        bool first = true;                                                                    \
        auto pos = 0;                                                                         \
        auto iterator0 = compress_mapping.find(item.name);                                    \
        if (iterator0 == compress_mapping.end()) {                                            \
            iterator0 = compress_mapping.insert({item.name, Index::CompressMapping{}}).first; \
        }                                                                                     \
        for (auto i = static_cast<uint16_t>(names.size()); i > 0; i--) {                      \
            if (!first)                                                                       \
                name = "." + name;                                                            \
            name = names[i - 1] + name;                                                       \
            auto iterator1 = compress_string_index.find(name);                                \
            if (iterator1 == compress_string_index.end()) {                                   \
                auto tmp = static_cast<uint16_t>(index - pos);                                \
                pos++;                                                                        \
                compress_string_index[name] = {tmp, name, 0};                                 \
                iterator0->second.emplace(tmp);                                               \
            } else {                                                                          \
                iterator0->second.emplace(iterator1->second.index);                           \
            }                                                                                 \
            first = false;                                                                    \
        }                                                                                     \
    }

    BUILD_INDEX(questions);
    BUILD_INDEX(answers);
    BUILD_INDEX(authorities);
    BUILD_INDEX(additionals);
#undef BUILD_INDEX
    // 索引换键
    compress_index.reserve(compress_string_index.size());
    std::transform(
        compress_string_index.begin(),
        compress_string_index.end(),
        std::back_inserter(compress_index),
        [](const std::pair<const std::string, CompressIndex> &item) {
            return item.second;
        }
    );
    compress_string_index.clear();
}

std::vector<DnsPackage::Index::CompressIndex *> DnsPackage::Index::getSortedIndexes(const std::set<uint16_t> &indexes) {
    std::vector<CompressIndex *> cached_items;
    cached_items.reserve(indexes.size());
    for (size_t i = 0; i < compress_index.size(); i++) {
        if (indexes.contains(compress_index[i].index)) {
            cached_items.push_back(&compress_index[i]);
        }
    }
    std::sort(cached_items.begin(), cached_items.end(), [](const CompressIndex *a, const CompressIndex *b) {
        return a->name.length() > b->name.length();
    });
    return cached_items;
}

void DnsPackage::Index::updatePos(std::vector<CompressIndex *> &indexes, uint16_t offset) {
    std::string fullname;
    uint16_t offset1 = 0;
    bool first = true;
    for (auto &&index: indexes) {
        if (index->pos != 0) {
            break;
        }
        if (!first) {
            offset1 = fullname.length() - index->name.length();
        } else {
            fullname = index->name;
        }
        index->pos = offset + offset1;
        first = false;
    }
}

void DnsPackage::Index::clearPos() const {
    for (auto index: compress_index) {
        index.pos = 0;
    }
}

inline uint16_t createPointer(uint16_t pos) {
    return (0xC000 | (pos & 0x3FFF));
}

std::string DnsPackage::Index::encodeWords(const std::string &fullname, const std::set<uint16_t> &indexes, uint16_t base_offset) {
    text::StringBuilder builder(128);
    auto cached_items = getSortedIndexes(indexes);
    auto words = text::StringBuilder::split(fullname, ".");
    for (size_t i = 0; i < cached_items.size(); i++) {
        auto &cached_item = cached_items[i];
        auto &word = words[i];
        if (cached_item->pos == 0) {
            builder.append(static_cast<uint8_t>(word.size()));
            builder.append(word);
        } else {
            uint16_t pos = createPointer(cached_item->pos);
            pos = ToBigEndian16(pos);
            builder.append(reinterpret_cast<const char *>(&pos), 2);
            break;
        }
    }
    builder.append('\0');
    updatePos(cached_items, base_offset);
    return builder.toString();
}
