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

/// @file DES.h
/// @brief DES algorithm
/// @author kaoru
/// @date April 13, 2024

#pragma once

#include <sese/Config.h>
#include <sese/security/evp/Crypter.h>

namespace sese::security::evp {

/// @brief DES algorithm
/// @deprecated The DES algorithm has been removed from OpenSSL
class SESE_DEPRECATED_WITH("The DES algorithm has been removed from OpenSSL") DESEncrypter final : public Crypter {
public:
    /// \brief DES algorithm type
    enum class Type {
        ECB,
        CBC,
        CFB1,
        CFB8,
        CFB64,
        EDE,
        EDE3,
    };

    DESEncrypter(Type type, const std::array<unsigned char, 8> &key);

    DESEncrypter(Type type, const std::string &key);

    ~DESEncrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx;
    unsigned char key[8];
};

/// @brief DES algorithm
/// @deprecated The DES algorithm has been removed from OpenSSL
class SESE_DEPRECATED_WITH("The DES algorithm has been removed from OpenSSL") DESDecrypter final : public Crypter {
public:
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    using Type = DESEncrypter::Type; // NOLINT
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    DESDecrypter(Type type, const std::array<unsigned char, 8> &key);

    DESDecrypter(Type type, const std::string &key);

    ~DESDecrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx;
    unsigned char key[8];
};

} // namespace sese::security::evp