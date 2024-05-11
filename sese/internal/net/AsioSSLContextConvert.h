#pragma once

#include "sese/security/SSLContext.h"

#include <asio/ssl.hpp>
#include <openssl/ssl.h>

namespace sese::internal::net {
/// 调用此函数将意味着将 context 的所有权转移至新转换的对象，旧的对象将不再有效
/// @param context 上下文
/// @return 新的 asio 上下文对象
inline asio::ssl::context convert(std::unique_ptr<security::SSLContext> context) {
    return asio::ssl::context(static_cast<SSL_CTX *>(context->release()));
}
} // namespace sese::internal::net
