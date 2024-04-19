/**
* @file Context.h
* @author kaoru
* @version 0.1
* @brief EVP 上下文基类
* @date 2023年9月13日
*/

#pragma once

#include <sese/Config.h>

namespace sese::security::evp {

/// EVP 上下文基类
class API Context {
public:
    using Ptr = std::unique_ptr<Context>;

    Context() = default;

    virtual ~Context() noexcept = default;

    virtual void update(const void *buffer, size_t len) noexcept = 0;

    virtual void final() noexcept = 0;

    virtual void *getResult() noexcept = 0;

    virtual size_t getLength() noexcept = 0;
};

} // namespace sese::security::evp