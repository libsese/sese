// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/convert/Compressor.h"
#include "zlib.h"

#include <algorithm>

sese::Compressor::Compressor(sese::CompressionType type, size_t level, size_t buffer_size) {
    stream = new z_stream;
    auto stm = static_cast<z_stream *>(stream);
    stm->zalloc = nullptr;
    stm->zfree = nullptr;
    stm->opaque = nullptr;
    stm->avail_in = 0;
    stm->next_in = nullptr;
    stm->avail_out = 0;
    stm->next_out = nullptr;

    cap = buffer_size;
    buffer = new unsigned char[buffer_size];

    level = std::min<size_t>(9, level); // GCOVR_EXCL_LINE
    deflateInit2(stm, static_cast<int>(level), Z_DEFLATED, MAX_WBITS + static_cast<int>(type), MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
}

sese::Compressor::~Compressor() {
    delete[] buffer; // GCOVR_EXCL_LINE

    auto stm = static_cast<z_stream *>(stream);
    ::deflateEnd(stm);
    delete stm; // GCOVR_EXCL_LINE
    stream = nullptr;
}

void sese::Compressor::input(const void *input, unsigned int input_size) {
    auto stm = static_cast<z_stream *>(stream);
    stm->avail_in = input_size;
    stm->next_in = const_cast<unsigned char *>(static_cast<const unsigned char *>(input));
    stm->avail_out = static_cast<unsigned int>(cap);
    stm->next_out = static_cast<unsigned char *>(buffer);
}

// GCOVR_EXCL_START
int sese::Compressor::deflate(OutputStream *out) {
    auto stm = static_cast<z_stream *>(stream);
    // The output buffer fails to fully output and continues to output
    if (length != 0) {
        auto last = length - read;
        auto real_wrote = out->write(buffer + read, last);
        if (last != real_wrote) {
            // The output buffer is not fully output
            read += real_wrote;
            return Z_BUF_ERROR;
        } else {
            length = 0;
            read = 0;

            stm->avail_out = static_cast<unsigned int>(cap);
            stm->next_out = buffer;
        }
    }

    // Enter buffer is not parsed completely, continue parsing
    int ret;
    do {
        ret = ::deflate(stm, Z_FINISH);
        if (ret == Z_STREAM_ERROR)
            return Z_STREAM_ERROR;

        auto wrote = static_cast<int>(cap) - stm->avail_out;
        auto real_wrote = out->write(buffer, wrote);
        if (wrote != real_wrote) {
            // The output buffer is not fully output
            length = wrote;
            read = real_wrote;
            return Z_BUF_ERROR;
        } else {
            stm->avail_out = static_cast<unsigned int>(cap);
            stm->next_out = buffer;
        }
    } while (ret != Z_STREAM_END);

    return Z_OK;
}

int sese::Compressor::reset() {
    length = 0;
    read = 0;
    auto stm = static_cast<z_stream *>(stream);
    return deflateReset(stm);
}
// GCOVR_EXCL_STOP

size_t sese::Compressor::getTotalIn() const {
    auto stm = static_cast<z_stream *>(stream);
    return stm->total_in;
}

size_t sese::Compressor::getTotalOut() const {
    auto stm = static_cast<z_stream *>(stream);
    return stm->total_out - length + read;
}