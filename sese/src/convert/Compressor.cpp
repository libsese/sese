#include "sese/convert/Compressor.h"
#include "zlib.h"

#include <algorithm>

sese::Compressor::Compressor(sese::CompressionType type, size_t level) {
    stream = new z_stream;
    auto stm = (z_stream *) stream;
    stm->zalloc = nullptr;
    stm->zfree = nullptr;
    stm->opaque = nullptr;
    stm->avail_in = 0;
    stm->next_in = nullptr;
    stm->avail_out = 0;
    stm->next_out = nullptr;

    level = std::min<size_t>(9, level);
    deflateInit2(stm, level, Z_DEFLATED, MAX_WBITS + (int) type, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
}

sese::Compressor::~Compressor() {
    auto stm = (z_stream *) stream;
    ::deflateEnd(stm);
    delete stm;
}

int sese::Compressor::deflate(const void *src, size_t srcLen, void *dst, size_t dstLen) const {
    auto stm = (z_stream *) stream;
    stm->avail_in = (unsigned int) srcLen;
    stm->next_in = (unsigned char *) src;
    stm->avail_out = (unsigned int) dstLen;
    stm->next_out = (unsigned char *) dst;
    auto rt = ::deflate(stm, Z_FINISH);
    auto wrote = stm->total_out;
    if (rt < 0) {
        deflateReset(stm);
        return rt;
    } else {
        deflateReset(stm);
        return (int) wrote;
    }
}