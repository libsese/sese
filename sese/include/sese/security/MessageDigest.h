/// \file MessageDigest.h
/// \brief 基于 OPENSSL::CRYPTO 的信息摘要接口
/// \author kaoru
/// \version 0.1
/// \date 2023年7月25日

#pragma once

#include "sese/util/NotInstantiable.h"
#include "sese/util/InputStream.h"

#include <memory>

namespace sese::security {
    class MessageDigest : public NotInstantiable {
    public:
        enum class Type {
            // MD4,
            MD5,
            SHA1,
            SHA224,
            SHA256,
            SHA384,
            SHA512
        };

        static std::unique_ptr<char[]> digest(Type type, InputStream *input, bool isCap = false) noexcept;

    private:
        // static void digestMD4(char *str, InputStream *input, bool isCap) noexcept;
        static void digestMD5(char *str, InputStream *input, bool isCap) noexcept;

        static void digestSH1(char *str, InputStream *input, bool isCap) noexcept;
        static void digestSHA224(char *str, InputStream *input, bool isCap) noexcept;
        static void digestSHA256(char *str, InputStream *input, bool isCap) noexcept;
        static void digestSHA384(char *str, InputStream *input, bool isCap) noexcept;
        static void digestSHA512(char *str, InputStream *input, bool isCap) noexcept;
    };
}// namespace sese::security