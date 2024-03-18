#pragma once

#include "sese/security/SSLContext.h"

#include <malloc.h>
#include <asio/ssl.hpp>
#include <openssl/ssl.h>

namespace sese::internal::net {
inline asio::ssl::context convert(const sese::security::SSLContext::Ptr &context) {
    return asio::ssl::context(static_cast<SSL_CTX *>(context->getContext()));
}
} // namespace sese::internal::net
