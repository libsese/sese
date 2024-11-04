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
