#pragma once
#include <sese/Stream.h>

namespace sese {
    struct SHA1Context {
        uint32_t h[5]{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        uint64_t total = 0;
    };

    class API SHA1Util {
    public:
        static bool encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;

        static std::unique_ptr<char []> encode(const InputStream::Ptr &input, bool isCap = true) noexcept;
    };
}// namespace sese