#include "sese/net/http/Huffman.h"
#include "sese/net/http/HPACK.h"

#include <limits>

using namespace sese::net::http;

huffman_node_t::huffman_node_t(
    huffman_node_t *l,
    huffman_node_t *r,
    int16_t c
) noexcept : m_left(l),
             m_right(r),
             m_code(c) {
}

huffman_tree_t::huffman_tree_t() noexcept : m_root(new huffman_node_t) {
    for (std::size_t idx = 0; idx < HUFFMAN_TABLE.size(); idx++) {
        const bits_t &bits = HUFFMAN_TABLE.at(idx);
        huffman_node_t *current = m_root;

        for (const auto &bit: bits) {
            if (bit) {
                if (nullptr == current->right()) {
                    current->right(new huffman_node_t);
                }
                current = current->right();
            } else {
                if (nullptr == current->left()) {
                    current->left(new huffman_node_t);
                }

                current = current->left();
            }
        }

        current->code(static_cast<int16_t>(idx));
    }
}

huffman_tree_t::~huffman_tree_t() noexcept {
    delete_node(m_root);
}

void huffman_tree_t::delete_node(huffman_node_t *n) noexcept {
    if (nullptr != n->right()) {
        delete_node(n->right());
    }
    if (nullptr != n->left()) {
        delete_node(n->left());
    }

    delete n;
}

std::optional<std::string> huffman_tree_t::decode(const char *src, size_t len) {
    std::string dst;
    huffman_node_t *current(m_root);

    if (len > std::numeric_limits<unsigned int>::max()) {
        // throw std::invalid_argument("HPACK::huffman_tree_t::decode(): Overly long input string");
        return std::nullopt;
    }

    for (unsigned int idx = 0; idx < len; ++idx) {
        bool has_value = false;
        for (int8_t j = 7; j >= 0; j--) {
            auto result = src[idx] & (1 << j);
            if (result > 0) {
                if (nullptr == current->right()) {
                    // throw std::runtime_error(
                    //        "HPACK::huffman_tree_t::decode(): Internal state error (right == nullptr)");
                    return std::nullopt;
                }
                current = current->right();
            } else {
                if (nullptr == current->left()) {
                    // throw std::runtime_error("HPACK::huffman_tree_t::decode(): Internal state error (left == nullptr)");
                    return std::nullopt;
                }
                current = current->left();
            }
            if (current->code() >= 0) {
                int16_t code = current->code();

                if (257 == code) {
                    dst += static_cast<char>((code & 0xFF00) >> 8 & 0xFF);
                }

                dst += static_cast<char>(code & 0xFF);
                current = m_root;
                has_value = true;
            }
        }
        if (!has_value) {
            return std::nullopt;
        }
    }

    return dst;
}

huffman_encoder_t::huffman_encoder_t() noexcept : m_byte(0), m_count(8) {
}

bool huffman_encoder_t::write_bit(uint8_t bit) noexcept {
    m_byte |= bit;
    m_count--;

    if (0 == m_count) {
        m_count = 8;
        return true;
    } else
        m_byte <<= 1;

    return false;
}

std::vector<uint8_t> huffman_encoder_t::encode(std::vector<uint8_t> &src) noexcept {
    std::vector<uint8_t> ret(0);

    for (auto &byte: src) {
        bits_t bits = HUFFMAN_TABLE.at(byte);

        for (decltype(auto) bit: bits) {
            if (write_bit(bit)) {
                ret.push_back(m_byte);
                m_byte = 0;
                m_count = 8;
            }
        }
    }

    // Apparently the remainder unused bits
    // are to be set to 1, some sources refer
    // to this as the EOS bit, but the code
    // for EOS is like 30-bits of 1's so its
    // clearly not the EOS code.
    if (8 != m_count && 0 != m_count) {
        m_byte = (m_byte << (m_count - 1));
        m_byte |= (0xFF >> (8 - m_count));
        ret.push_back(m_byte);
        m_byte = 0;
        m_count = 8;
    }

    return ret;
}

std::vector<uint8_t> huffman_encoder_t::encode(const std::string &src) noexcept {
    std::vector<uint8_t> s(src.begin(), src.end());
    return encode(s);
}

std::vector<uint8_t> huffman_encoder_t::encode(const char *ptr) noexcept {
    std::string str(ptr);

    if (nullptr == ptr) {
        // throw std::invalid_argument("HPACK::huffman_encoder_t::encode(): Invalid nullptr parameter");
        return {};
    }

    return encode(str);
}
