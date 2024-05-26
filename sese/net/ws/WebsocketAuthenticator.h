/// \file WebsocketAuthenticator.h
/// \brief Websocket 验证套件
/// \author kaoru
/// \version 0.1
/// \date 2023年8月10日

#pragma once

#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"
#include <utility>

namespace sese::net::ws {

/// Websocket 验证器
class API WebsocketAuthenticator final : public NotInstantiable {
public:
    WebsocketAuthenticator() = delete;

    /// 生成密钥对
    /// \return 密钥对
    static std::pair<std::unique_ptr<char[]>, std::unique_ptr<char[]>> generateKeyPair() noexcept;

    /// 验证密钥对
    /// \param key 密钥
    /// \param result 结果
    /// \return 验证结果
    static bool verify(const char *key, const char *result) noexcept;

    /// 由密钥生成结果
    /// \param key 密钥
    /// \return 结果
    static std::unique_ptr<char[]> toResult(const char *key) noexcept;

private:
    static const char *append_string;

    /// 生成二进制 key
    static void generateKey(uint8_t key[16]);
};
} // namespace sese::net::ws