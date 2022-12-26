/**
 * \file SHA256Util.h
 * \author kaoru
 * \date 2022/11/15
 * \version 0.1
 * \brief 非硬件指令加速的 SHA256 摘要算法
 */

#pragma once
#include <sese/Stream.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    struct Bitset32;

    /// \brief 非硬件指令加速的 SHA256 摘要算法，对性能有要求请不要使用此工具类
    class API SHA256Util {
    public:
        static bool encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;

        static std::unique_ptr<char []> encode(const InputStream::Ptr &input, bool isCap = true) noexcept;

    private:
        /// 常数序列
        static const Bitset32 k[64];

        static void structure(Bitset32 *block) noexcept;
        static void encode(Bitset32 value[8], Bitset32 *block, Bitset32 k) noexcept;
    };
}