/// \file MessageDigest.h
/// \brief 基于 OPENSSL::CRYPTO 的信息摘要接口
/// \author kaoru
/// \version 0.1
/// \date 2023年7月25日

#pragma once

#include "sese/util/NotInstantiable.h"
#include "sese/io/InputStream.h"

#include <memory>

namespace sese::security {
/// 基于 OPENSSL::CRYPTO 的信息摘要接口
class MessageDigest : public NotInstantiable {
public:
    using InputStream = io::InputStream;

    enum class Type {
        // MD4,
        MD5,
        SHA1,
        // SHA224,
        SHA256,
        SHA384,
        SHA512
    };

    static std::string digest(Type type, InputStream *input, bool is_cap = false) noexcept;

private:
    // static void digestMD4(char *str, InputStream *input, bool isCap) noexcept;
    static void digestMD5(char *str, InputStream *input, bool is_cap) noexcept;

    static void digestSH1(char *str, InputStream *input, bool is_cap) noexcept;
    // static void digestSHA224(char *str, InputStream *input, bool isCap) noexcept;
    static void digestSHA256(char *str, InputStream *input, bool is_cap) noexcept;
    static void digestSHA384(char *str, InputStream *input, bool is_cap) noexcept;
    static void digestSHA512(char *str, InputStream *input, bool is_cap) noexcept;
};
} // namespace sese::security