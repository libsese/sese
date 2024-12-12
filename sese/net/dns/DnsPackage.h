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

/// @file DnsPackage.h
/// @brief DNS Package Object
/// @author kaoru
/// @date October 30, 2024

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace sese::net::dns {

/// \brief DNS Package Object
class DnsPackage {
public:
    /// \brief DNS Flags Utility
    struct Flags {
        bool qr = false;
        uint8_t opcode = 0;
        bool aa = false;
        bool tc = false;
        bool rd = false;
        bool ra = false;
        uint8_t z = false;
        uint8_t rcode = false;

        [[nodiscard]] uint16_t encode() const;

        void decode(uint16_t flags);
    };

    class Index;

    /// \brief DNS Question
    struct Question {
        std::string name;
        uint16_t type;
        uint16_t class_;
    };

    /// \brief DNS Answer
    struct Answer {
        std::string name;
        uint16_t type;
        uint16_t class_;
        uint32_t ttl;
        uint16_t data_length;
        std::unique_ptr<uint8_t[]> data{};
    };

    /// \brief DNS Authority Answer
    using Authority = Answer;
    /// \brief DNS Additional Answer
    using Additional = Answer;

private:
    struct DnsHeader {
        uint16_t id;
        uint16_t flags;
    } header{};

    std::vector<Question> questions;
    std::vector<Answer> answers;
    std::vector<Authority> authorities;
    std::vector<Additional> additionals;

    DnsPackage() = default;

    /// Decode DNS name
    /// @warning This function performs recursive decoding of multi-level pointers
    /// @param buffer Complete buffer
    /// @param length Size of buffer
    /// @param offset Offset to be decoded, ends at 00 position after decoding
    /// @return Decoded string
    static std::string decodeWords(const uint8_t *buffer, size_t length, size_t &offset); // NOLINT

    static bool decodeAnswers(std::vector<Answer> &answers, size_t expect_size, const uint8_t *buffer, size_t length, size_t &pos);

    static std::string encodeWords(const std::string &fullname);

    static bool encodeQuestions(const std::vector<Question> &questions, void *buffer, size_t &length, Index &index, size_t offset);

    static bool encodeAnswers(const std::vector<Answer> &answers, void *buffer, size_t &length, Index &index, size_t offset);

public:
    /// \brief DNS Package Compression Index
    class Index {
        friend class DnsPackage;
        using CompressMapping = std::set<uint16_t>;
        struct CompressIndex {
            uint16_t index;
            std::string name;
            uint16_t pos;
        };

        std::vector<CompressIndex> compress_index;
        std::map<std::string, CompressMapping> compress_mapping;

        Index(
            std::vector<Question> &questions,
            std::vector<Answer> &answers,
            std::vector<Authority> &authorities,
            std::vector<Additional> &additionals
        );

        [[nodiscard]] std::vector<CompressIndex *> getSortedIndexes(const std::set<uint16_t> &indexes);

        static void updatePos(std::vector<CompressIndex *> &indexes, uint16_t offset);

        void clearPos() const;

        std::string encodeWords(const std::string &fullname, const std::set<uint16_t> &indexes, uint16_t base_offset);

    public:
        Index() = default;
    };

    using Ptr = std::shared_ptr<DnsPackage>;

    /// @return Empty DNS Package Compression Index
    static Ptr new_();

    /// Parse DNS raw package
    /// @param buffer DNS package buffer
    /// @param length Buffer size
    /// @return Returns object on success, otherwise returns nullptr
    static Ptr decode(const uint8_t *buffer, size_t length);

    /// Get DNS Package Compression Index
    /// @return Index
    Index buildIndex();

    /// Encode DNS package to buffer
    /// @param buffer Output buffer
    /// @param length Output buffer size, returns actual size used on success; if input is 0, calculates required buffer size without actual packaging
    /// @param index DNS Package Compression Index
    /// @return Whether encoding was successful
    bool encode(void *buffer, size_t &length, Index &index) const;

    [[nodiscard]] std::vector<Question> &getQuestions() { return questions; }
    [[nodiscard]] std::vector<Answer> &getAnswers() { return answers; }
    [[nodiscard]] std::vector<Authority> &getAuthorities() { return authorities; }
    [[nodiscard]] std::vector<Additional> &getAdditionals() { return additionals; }
    [[nodiscard]] auto getId() const { return header.id; }
    [[nodiscard]] auto getFlags() const { return header.flags; }

    void setId(uint16_t id) { header.id = id; }
    void setFlags(uint16_t flags) { header.flags = flags; }
};


} // namespace sese::net::dns