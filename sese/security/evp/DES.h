/// @file DES.h
/// @brief DES 加解密
/// @author kaoru
/// @date 2024年04月13日

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

/// @brief DES 加密
class DESEncrypter final : public Crypter {
public:
    /// @brief DES 加密类型
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

/// @brief DES 解密
class DESDecrypter final : public Crypter {
public:
    using Type = DESEncrypter::Type;

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