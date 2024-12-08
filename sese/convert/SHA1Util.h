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
 * \file SHA1Util.h
 * \author kaoru
 * \date December 26, 2022
 * \version 0.1
 * \brief SHA1 digest algorithm without hardware instruction acceleration
 */

#pragma once

#include "sese/io/Stream.h"

namespace sese {

/// SHA1 context
struct SHA1Context {
    uint32_t h[5]{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    uint64_t total = 0;
};

/// SHA1 algorithm tool
class  SHA1Util {
public:
    using OutputStream = sese::io::OutputStream;
    using InputStream = sese::io::InputStream;

    static void encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;
    static void encode(InputStream *input, OutputStream *output) noexcept;

    static std::unique_ptr<char[]> encode(const InputStream::Ptr &input, bool is_cap = true) noexcept;
    static std::unique_ptr<char[]> encode(InputStream *input, bool is_cap = true) noexcept;
};
} // namespace sese