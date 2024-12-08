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
 * @file PercentConverter.h
 * @author kaoru
 * @date April 18, 2022
 * @brief Percent sign encoding converter
 */

#pragma once

#include "sese/io/OutputStream.h"
#include "sese/util/NotInstantiable.h"

#include <set>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese {

/**
 * @brief Percent sign encoding converter
 */
class  PercentConverter final : public NotInstantiable {
public:
    using OutputStream = io::OutputStream;

    PercentConverter() = delete;

    static void encode(const char *src, const OutputStream::Ptr &dest);

    static bool decode(const char *src, const OutputStream::Ptr &dest);

    static void encode(const char *src, OutputStream *dest);

    static bool decode(const char *src, OutputStream *dest);

    static std::string encode(const char *src);

    /// Decode string
    /// \param src String to be decoded
    /// \retval {} Decoding failed
    static std::string decode(const char *src);

private:
    static const std::set<char> URL_EXCLUDE_CHARS;
};
} // namespace sese