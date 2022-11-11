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
        static const uint32_t k[64];

        static void structure(Bitset32 *block) noexcept;
        static void encode(uint32_t hash[8], uint32_t value[8], uint32_t *block, uint32_t k) noexcept;
    };
}