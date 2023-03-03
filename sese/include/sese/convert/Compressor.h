#pragma once

#include "sese/convert/ZlibConfig.h"

namespace sese {

    class API Compressor {
    public:
        explicit Compressor(CompressionType type, size_t level);

        virtual ~Compressor();

        int deflate(const void *src, size_t srcLen, void *dst, size_t dstLen) const;

    private:
        void *stream;
    };
}// namespace sese