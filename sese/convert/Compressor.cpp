#include "sese/convert/Compressor.h"
#include "zlib.h"

#include <algorithm>

sese::Compressor::Compressor(sese::CompressionType type, size_t level, size_t buffer_size) {
    stream = new z_stream;
    auto stm = (z_stream *) stream;
    stm->zalloc = nullptr;
    stm->zfree = nullptr;
    stm->opaque = nullptr;
    stm->avail_in = 0;
    stm->next_in = nullptr;
    stm->avail_out = 0;
    stm->next_out = nullptr;

    cap = buffer_size;
    buffer = new unsigned char[buffer_size];

    level = std::min<size_t>(9, level);
    deflateInit2(stm, (int) level, Z_DEFLATED, MAX_WBITS + (int) type, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
}

sese::Compressor::~Compressor() {
    delete[] buffer; // GCOVR_EXCL_LINE

    auto stm = (z_stream *) stream;
    ::deflateEnd(stm);
    delete stm; // GCOVR_EXCL_LINE
    stream = nullptr;
}

void sese::Compressor::input(const void *input, unsigned int input_size) {
    auto stm = (z_stream *) stream;
    stm->avail_in = input_size;
    stm->next_in = (unsigned char *) input;
    stm->avail_out = (unsigned int) cap;
    stm->next_out = (unsigned char *) buffer;
}

// GCOVR_EXCL_START
int sese::Compressor::deflate(OutputStream *out) {
    auto stm = (z_stream *) stream;
    // 输出 buffer 未能完全输出，继续输出
    if (length != 0) {
        auto last = length - read;
        auto real_wrote = out->write(buffer + read, last);
        if (last != real_wrote) {
            // 输出 buffer 未能全部输出
            read += real_wrote;
            return Z_BUF_ERROR;
        } else {
            length = 0;
            read = 0;

            stm->avail_out = (unsigned int) cap;
            stm->next_out = buffer;
        }
    }

    // 输入 buffer 未解析完全，继续解析
    int ret;
    do {
        ret = ::deflate(stm, Z_FINISH);
        if (ret == Z_STREAM_ERROR)
            return Z_STREAM_ERROR;

        auto wrote = (int) cap - stm->avail_out;
        auto real_wrote = out->write(buffer, wrote);
        if (wrote != real_wrote) {
            // 输出 buffer 未能全部输出
            length = wrote;
            read = real_wrote;
            return Z_BUF_ERROR;
        } else {
            stm->avail_out = (unsigned int) cap;
            stm->next_out = buffer;
        }
    } while (ret != Z_STREAM_END);

    return Z_OK;
}

int sese::Compressor::reset() {
    length = 0;
    read = 0;
    auto stm = (z_stream *) stream;
    return deflateReset(stm);
}
// GCOVR_EXCL_STOP

size_t sese::Compressor::getTotalIn() const {
    auto stm = (z_stream *) stream;
    return stm->total_in;
}

size_t sese::Compressor::getTotalOut() const {
    auto stm = (z_stream *) stream;
    return stm->total_out - length + read;
}