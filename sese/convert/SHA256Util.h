/**
 * \file SHA256Util.h
 * \author kaoru
 * \date 2022/11/15
 * \version 0.1
 * \brief 非硬件指令加速的 SHA256 摘要算法
 */

#pragma once
#include "sese/io/Stream.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

struct Bitset32;

/// \brief 非硬件指令加速的 SHA256 摘要算法，对性能有要求请不要使用此工具类
class  SHA256Util {
public:
    using OutputStream = sese::io::OutputStream;
    using InputStream = sese::io::InputStream;

    static void encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;
    static void encode(InputStream *input, OutputStream *output) noexcept;

    static std::unique_ptr<char[]> encode(const InputStream::Ptr &input, bool is_cap = true) noexcept;
    static std::unique_ptr<char[]> encode(InputStream *input, bool is_cap = true) noexcept;

private:
    /// 常数序列
    static const Bitset32 K[64];

    static void structure(Bitset32 *block) noexcept;
    static void encode(Bitset32 value[8], Bitset32 *block, Bitset32 k) noexcept;
};
} // namespace sese