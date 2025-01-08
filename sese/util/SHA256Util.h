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
 * \file SHA256Util.h
 * \author kaoru
 * \date November 15, 2022
 * \version 0.1
 * \brief SHA256 digest algorithm without hardware instruction acceleration
 */

#pragma once

#include "sese/io/Stream.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

struct Bitset32;

/// \brief SHA256 digest algorithm without hardware instruction acceleration,
/// do not use this utility class if performance is a concern
class SHA256Util {
public:
    using OutputStream = sese::io::OutputStream;
    using InputStream = sese::io::InputStream;

    static void encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;
    static void encode(InputStream *input, OutputStream *output) noexcept;

    static std::unique_ptr<char[]> encode(const InputStream::Ptr &input, bool is_cap = true) noexcept;
    static std::unique_ptr<char[]> encode(InputStream *input, bool is_cap = true) noexcept;

private:
    /// Constant sequences
    static const Bitset32 K[64];

    static void structure(Bitset32 *block) noexcept;
    static void encode(Bitset32 value[8], Bitset32 *block, Bitset32 k) noexcept;
};
} // namespace sese