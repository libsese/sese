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
 * @file MD5Util.h
 * @brief MD5 Algorithm Class
 * @author kaoru
 * @date April 22, 2022
 */

#pragma once

#include "sese/io/Stream.h"

namespace sese {

/**
 * @brief MD5 Algorithm Class
 */
class  MD5Util {
public:
    using OutputStream = sese::io::OutputStream;
    using InputStream = sese::io::InputStream;

    /**
     * @brief Generate MD5 information
     * @param input Source of digest
     * @param output MD5 data output
     */
    static void encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;
    /**
     * @brief Generate MD5 information
     * @param input Source of digest
     * @param output MD5 data output
     */
    static void encode(InputStream *input, OutputStream *output) noexcept;

    /**
     * @brief Generate a 32-bit uppercase MD5 string
     * @param input Source of digest
     * @param is_cap Indicates whether the string letters are uppercase
     * @return Returns the generated string, or nullptr if generation fails
     */
    static std::unique_ptr<char[]> encode(const InputStream::Ptr &input, bool is_cap = true) noexcept;
    /**
     * @brief Generate a 32-bit uppercase MD5 string
     * @param input Source of digest
     * @param is_cap Indicates whether the string letters are uppercase
     * @return Returns the generated string, or nullptr if generation fails
     */
    static std::unique_ptr<char[]> encode(InputStream *input, bool is_cap = true) noexcept;

private:
    static void transform(uint32_t *res, uint8_t *buffer) noexcept;

    static const uint32_t A = 0x67452301;
    static const uint32_t B = 0xefcdab89;
    static const uint32_t C = 0x98badcfe;
    static const uint32_t D = 0x10325476;

    static const uint32_t S11 = 7;
    static const uint32_t S12 = 12;
    static const uint32_t S13 = 17;
    static const uint32_t S14 = 22;

    static const uint32_t S21 = 5;
    static const uint32_t S22 = 9;
    static const uint32_t S23 = 14;
    static const uint32_t S24 = 20;

    static const uint32_t S31 = 4;
    static const uint32_t S32 = 11;
    static const uint32_t S33 = 16;
    static const uint32_t S34 = 23;

    static const uint32_t S41 = 6;
    static const uint32_t S42 = 10;
    static const uint32_t S43 = 15;
    static const uint32_t S44 = 21;

    constexpr static const unsigned char PADDING[64] = {0x80};

    static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) noexcept;
    static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) noexcept;
    static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) noexcept;
    static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) noexcept;

    static inline uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
    static inline uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
    static inline uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
    static inline uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
};
} // namespace sese