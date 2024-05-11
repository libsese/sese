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
class API SSLContextBuilder final : public NotInstantiable {
public:
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