#pragma once
#include <sese/Stream.h>

namespace sese {

    struct Bitset32;

    class API SHA256Util {
    public:
        static bool encode(const Stream::Ptr &input, const Stream::Ptr &output) noexcept;

        static std::unique_ptr<char []> encode(const Stream::Ptr &input, bool isCap = true) noexcept;

    private:
        /// 常数序列
        static const Bitset32 k[64];

        static void structure(Bitset32 *block) noexcept;
        static void encode(Bitset32 value[8], Bitset32 *block, Bitset32 k) noexcept;
    };
}