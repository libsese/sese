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
 * @file Base64Converter.h
 * @author kaoru
 * @date April 18, 2022
 * @brief BASE64 converter
 */

#pragma once

#include "sese/io/Stream.h"
#include "sese/util/NotInstantiable.h"

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese {

/**
 * @brief BASE64 converter
 */
class Base64Converter final : public NotInstantiable {
public:
    using OutputStream = sese::io::OutputStream;
    using InputStream = sese::io::InputStream;

    Base64Converter() = delete;

    static void encode(const InputStream::Ptr &src, const OutputStream::Ptr &dest);
    static void encode(InputStream *src, OutputStream *dest);

    static void decode(const InputStream::Ptr &src, const OutputStream::Ptr &dest);
    static void decode(InputStream *src, OutputStream *dest);

public:
    using CodePage = const unsigned char *;

    /// Encode according to the Base62 code table
    /// \warning Note that the encoding method of this function is similar to Base64, but it is not standard Base62 encoding
    /// \param input Input stream
    /// \param output Output stream
    /// \return Encoding result
    static bool encodeBase62(InputStream *input, OutputStream *output) noexcept;
    /// \brief Decode according to the Base62 code table
    /// \warning Note that the decoding method of this function is similar to Base64, but it is not standard Base62 decoding
    /// \param input Input stream
    /// \param output Output stream
    /// \return Encoding result
    static bool decodeBase62(InputStream *input, OutputStream *output) noexcept;

public:
    static bool encodeInteger(size_t num, OutputStream *output) noexcept;
    static int64_t decodeBuffer(const unsigned char *buffer, size_t size) noexcept;
};
} // namespace sese