#pragma once

#include "sese/security/SSLContext.h"
#include <asio/ssl.hpp>

inline asio::ssl::context convert(const sese::security::SSLContext &context) {
    return asio::ssl::context(static_cast<SSL_CTX *>(context.getContext()));
}