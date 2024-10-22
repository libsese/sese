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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace sese::net::dns {

class DnsPackage {
    struct Question {
        std::string name;
        uint16_t type;
        uint16_t class_;
    };

    struct Answer {
        std::string name;
        uint16_t type;
        uint16_t class_;
        uint32_t ttl;
        uint16_t data_length;
        std::unique_ptr<uint8_t[]> data{};
    };

    using Authority = Answer;
    using Additional = Answer;

    struct DnsHeader {
        uint16_t id;
        uint16_t flags;
        /// 问题数
        uint16_t qdcount;
        /// 解析记录数
        uint16_t ancount;
        /// 权威记录数
        uint16_t nscount;
        /// 附加记录数
        uint16_t arcount;
    };

    std::vector<Question> questions;
    std::vector<Answer> answers;
    std::vector<Authority> authorities;
    std::vector<Additional> additionals;

    DnsPackage() = default;

    /// 解码 DNS 名称
    /// @warning 注意此函数会进行递归调用解码多级指针
    /// @param buffer 完整 buffer
    /// @param length buffer 大小
    /// @param offset 需要解码的偏移位置，解码完成后停留在 00 位置
    /// @return 解码后字符串
    static std::string decodeWords(const uint8_t *buffer, size_t length, size_t &offset); // NOLINT

    static bool decodeAnswers(std::vector<Answer> &answers, size_t expect_size, const uint8_t *buffer, size_t length, size_t &pos);

public:
    using Ptr = std::shared_ptr<DnsPackage>;

    static Ptr new_();

    static Ptr decode(const uint8_t *buffer, size_t length);

    // bool encode(const void *buffer, size_t length);

    [[nodiscard]] std::vector<Question> &getQuestions() { return questions; }
    [[nodiscard]] std::vector<Answer> &getAnswers() { return answers; }
    [[nodiscard]] std::vector<Authority> &getAuthorities() { return authorities; }
    [[nodiscard]] std::vector<Additional> &getAdditionals() {return additionals;}
};

} // namespace sese::net::dns