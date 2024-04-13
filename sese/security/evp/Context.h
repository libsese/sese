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

/// EVP 加解密上下文
class Crypter {
public:
    virtual ~Crypter() = default;

    /// 处理数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @param in 输入缓冲区指针
    /// @param in_len 输入缓存区的大小
    /// @return 返回处理的字节数，失败返回负数
    virtual int update(void *out, int &out_len, const void *in, int in_len) const noexcept;

    /// 处理最后的数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @return 返回处理的字节数，失败返回负数
    virtual int final(void *out, int &out_len) const noexcept;
};


} // namespace sese::security::evp