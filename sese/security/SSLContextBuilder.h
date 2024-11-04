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

/// \file SSLContextBuilder.h
/// \brief SSL 上下文构建器
/// \version 0.1
/// \author kaoru
/// \date 2023年7月25日

#pragma once

#include <sese/security/SSLContext.h>
#include <sese/util/NotInstantiable.h>

namespace sese::security {

/// SSL 上下文构建器
class  SSLContextBuilder final : public NotInstantiable {
public:
    SSLContextBuilder() = delete;

    /// @brief 为客户端构建 SSL 上下文
    /// @return 客户端 SSL 上下文
    static SSLContext::Ptr SSL4Client() noexcept;
    /// @brief 为服务端构建 SSL 上下文
    /// @return 服务的 SSL 上下文
    static SSLContext::Ptr SSL4Server() noexcept;
    /// @brief 为客户端构建 SSL 上下文
    /// @return 客户端 SSL 上下文
    static std::unique_ptr<SSLContext> UniqueSSL4Client() noexcept;
    /// @brief 为服务端构建 SSL 上下文
    /// @return 服务的 SSL 上下文
    static std::unique_ptr<SSLContext> UniqueSSL4Server() noexcept;
};
} // namespace sese::security

#define WIN32_LEAN_AND_MEAN