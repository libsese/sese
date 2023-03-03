#include "sese/convert/Decompressor.h"
#include "zlib.h"

sese::Decompressor::Decompressor(sese::CompressionType type) {
    stream = new z_stream;
    auto stm = (z_stream *) stream;
    stm->zalloc = nullptr;
    stm->zfree = nullptr;
    stm->opaque = nullptr;
    stm->avail_in = 0;
    stm->next_in = nullptr;
    stm->avail_out = 0;
    stm->next_out = nullptr;

    inflateInit2(stm, MAX_WBITS + (int) type);
}

sese::Decompressor::~Decompressor() {
    auto stm = (z_stream *) stream;
    ::inflateEnd(stm);
    delete stm;
}

int sese::Decompressor::inflate(const void *src, size_t srcLen, void *dst, size_t dstLen) const {
    auto stm = (z_stream *) stream;
    stm->avail_in = (unsigned int) srcLen;
    stm->next_in = (unsigned char *) src;
    stm->avail_out = (unsigned int) dstLen;
    stm->next_out = (unsigned char *) dst;
    auto rt = ::inflate(stm, Z_FINISH);
    auto wrote = stm->total_out;
    if (rt < 0) {
        return rt;
    } else {
        inflateReset(stm);
        return (int) wrote;
    }
}

int sese::Decompressor::inflate(void *dst, size_t dstLen) const {
    auto stm = (z_stream *) stream;
    stm->next_out = (unsigned char *) dst;
    stm->avail_out = (unsigned int) dstLen;
    auto wrote = stm->total_out;
    auto rt = ::inflate(stm, Z_FINISH);
    wrote = stm->total_out - wrote;
    if (rt < 0) {
        return rt;
    } else {
        inflateReset(stm);
        return (int) wrote;
    }
}