/// @file DES.h
/// @brief DES 加解密
/// @author kaoru
/// @date 2024年04月13日

#pragma once

#include <sese/Config.h>
#include <sese/security/evp/Crypter.h>

namespace sese::security::evp {

/// @brief DES 加密
/// @deprecated DES 加密已从 OpenSSL 中移除
class SESE_DEPRECATED_WITH("DES 加密已从 OpenSSL 中移除") DESEncrypter final : public Crypter {
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
/// @deprecated DES 加密已从 OpenSSL 中移除
class SESE_DEPRECATED_WITH("DES 加密已从 OpenSSL 中移除") DESDecrypter final : public Crypter {
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