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

/// \file Crypter.h
/// \brief Crypter interface
/// \author kaoru
/// \date April 22, 2024
/// \deprecated Please use the corresponding OpenSSL EVP methods directly

#pragma once

#include <sese/io/InputBuffer.h>

namespace sese::security::evp {

/// Crypter interface
class Crypter {
public:
    virtual ~Crypter() = default;

    /// \brief Process data
    /// \param out Pointer to the output buffer
    /// \param out_len Number of bytes in the output buffer
    /// \param in Pointer to the input buffer
    /// \param in_len Size of the input buffer
    /// \return Number of bytes processed, negative if failed
    virtual int update(void *out, int &out_len, const void *in, int in_len) const noexcept = 0;

    /// \brief Process final data
    /// \param out Pointer to the output buffer
    /// \param out_len Number of bytes in the output buffer
    /// \return Number of bytes processed, negative if failed
    virtual int final(void *out, int &out_len) const noexcept = 0;
};

/// \brief EVP encryption and decryption context
struct CrypterContext {
    using Ptr = std::unique_ptr<CrypterContext>;

    io::InputBuffer key;
    io::InputBuffer vector;
    const void *crypter_pointer{};
};

/// \brief Decrypter
class Decrypter final : public Crypter {
public:
    explicit Decrypter(const CrypterContext::Ptr &crypter_context);

    ~Decrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx_;
    const CrypterContext::Ptr &crypter_context_;
};

/// \brief Encrypter
class Encrypter final : public Crypter {
public:
    explicit Encrypter(const CrypterContext::Ptr &crypter_context);

    ~Encrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx_;
    const CrypterContext::Ptr &crypter_context_;
};

}