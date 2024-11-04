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

/// @file Crypter.h
/// @brief 加解器接口
/// @author kaoru
/// @date 2024年04月22日
/// @deprecated 请直接使用 OpenSSL 对应的 EVP 方法

#pragma once

#include <sese/io/InputBuffer.h>

namespace sese::security::evp {

/// EVP 加解密接口
class Crypter {
public:
    virtual ~Crypter() = default;

    /// 处理数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @param in 输入缓冲区指针
    /// @param in_len 输入缓存区的大小
    /// @return 返回处理的字节数，失败返回负数
    virtual int update(void *out, int &out_len, const void *in, int in_len) const noexcept = 0;

    /// 处理最后的数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @return 返回处理的字节数，失败返回负数
    virtual int final(void *out, int &out_len) const noexcept = 0;
};

/// EVP 加解密上下文
struct CrypterContext {
    using Ptr = std::unique_ptr<CrypterContext>;

    io::InputBuffer key;
    io::InputBuffer vector;
    /// 加密器指针，请勿手动修改该属性
    const void *crypter_pointer{};
};

/// @brief 解密器
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

/// @brief 加密器
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