#pragma once

#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"
#include <utility>

namespace sese::net::ws {
    class API WebSocketAuthenticator : public NotInstantiable {
    public:
        static std::pair<std::unique_ptr<char []>, std::unique_ptr<char []>> generateKeyPair() noexcept;

        static bool verify(const char *key, const char *result) noexcept;

        static std::unique_ptr<char []> toResult(const char *key) noexcept;

    private:
        static const char *APPEND_STRING;

        /// 生成二进制 key
        static void generateKey(uint8_t key[16]);
    };
}