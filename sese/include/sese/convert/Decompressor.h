#pragma once

#include "sese/convert/ZlibConfig.h"

namespace sese {

    class API Decompressor {
    public:
        explicit Decompressor(CompressionType type);

        virtual ~Decompressor();

        int inflate(const void *src, size_t srcLen, void *dst, size_t dstLen) const;

        int inflate(void *dst, size_t dstLen) const;

    private:
        void *stream;
    };
}// namespace sese