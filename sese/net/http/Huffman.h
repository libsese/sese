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
 * @file Huffman.h
 * @brief HTTP2 Huffman 解码与编码实现
 * @verbatim
   此文件来源于
   https://github.com/jnferguson/hpack-rfc7541.git
   部分
   @endverbatim
 * @bug 原实现存在数据截断的 bug，此处已经修复，参考 huffman_tree_t::decode
 */

#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace sese::net::http {

/// Huffman 解码节点
class huffman_node_t {
protected:
    huffman_node_t *m_left;
    huffman_node_t *m_right;
    int16_t m_code;

public:
    explicit huffman_node_t(huffman_node_t *l = nullptr, huffman_node_t *r = nullptr, int16_t c = -1) noexcept;

    virtual ~huffman_node_t() {
        m_left = nullptr;
        m_right = nullptr;
        m_code = 0;
    }

    [[nodiscard]] int16_t code() const { return m_code; }

    void code(int16_t c) { m_code = c; }

    huffman_node_t *left() { return m_left; }

    void left(huffman_node_t *l) { m_left = l; }

    huffman_node_t *right() { return m_right; }

    void right(huffman_node_t *r) { m_right = r; }
};

/// Huffman 树
class huffman_tree_t {
protected:
    huffman_node_t *m_root;

    void delete_node(huffman_node_t *n) noexcept;

public:
    huffman_tree_t() noexcept;

    virtual ~huffman_tree_t() noexcept;

    std::optional<std::string> decode(const char *src, size_t len) const;
};

/// Huffman 编码器
class huffman_encoder_t {
private:
    uint8_t m_byte;
    uint8_t m_count;

protected:
    inline bool write_bit(uint8_t bit) noexcept;

public:
    huffman_encoder_t() noexcept;

    virtual ~huffman_encoder_t() noexcept = default;

    std::vector<uint8_t> encode(const std::vector<uint8_t> &src) noexcept;

    std::vector<uint8_t> encode(const std::string &src) noexcept;

    std::vector<uint8_t> encode(const char *ptr) noexcept;
};

using HuffmanEncoder = huffman_encoder_t;
using HuffmanDecoder = huffman_tree_t;

} // namespace sese::net::http