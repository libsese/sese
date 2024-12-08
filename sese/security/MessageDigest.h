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

/// \file MessageDigest.h
/// \brief Message digest interface based on OPENSSL::CRYPTO
/// \author kaoru
/// \version 0.1
/// \date July 25, 2023

#pragma once

#include "sese/util/NotInstantiable.h"
#include "sese/io/InputStream.h"

#include <memory>
#include <string>

namespace sese::security {
/// Message digest interface based on OPENSSL::CRYPTO
class MessageDigest final : public NotInstantiable {
public:
    using InputStream = io::InputStream;

    enum class Type {
        // MD4,
        MD5,
        SHA1,
        // SHA224,
        SHA256,
        SHA384,
        SHA512,
        SM3
    };

    MessageDigest() = delete;

    static std::string digest(Type type, InputStream *input, bool is_cap = false) noexcept;

private:
    // static void digestMD4(char *str, InputStream *input, bool isCap) noexcept;
    static void digestMD5(char *str, InputStream *input, bool is_cap) noexcept;

    static void digestSH1(char *str, InputStream *input, bool is_cap) noexcept;
    // static void digestSHA224(char *str, InputStream *input, bool isCap) noexcept;
    static void digestSHA256(char *str, InputStream *input, bool is_cap) noexcept;
    static void digestSHA384(char *str, InputStream *input, bool is_cap) noexcept;
    static void digestSHA512(char *str, InputStream *input, bool is_cap) noexcept;
    static void digestSM3(char *str, InputStream *input, bool is_cap) noexcept;
};
} // namespace sese::security