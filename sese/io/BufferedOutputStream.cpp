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

#include "sese/io/BufferedOutputStream.h"

using sese::io::BufferedOutputStream;

BufferedOutputStream::BufferedOutputStream(const OutputStream::Ptr &source, size_t buffer_size) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = buffer_size;
    this->buffer = malloc(cap);
}

BufferedOutputStream::~BufferedOutputStream() noexcept {
    free(buffer);
}

int64_t BufferedOutputStream::write(const void *buf, size_t length) {
    /*
     * If the number of bytes required to write needs to flush the cache less than twice,
     * then temporarily write to the cache to avoid frequent I/O operations;
     * If the number of bytes required to write needs to flush the cache two times or more,
     * then after processing the original cache, operate directly on the raw stream to reduce the number of copies
     */
    if (length <= this->cap) {
        if (this->cap - this->len >= length) {
            // The number of bytes is sufficient - no refresh is required
            memcpy(static_cast<char *>(this->buffer) + this->len, buf, length);
            this->len += length;
            return static_cast<int64_t>(length);
        } else {
            // Insufficient bytes - needs to be refreshed
            size_t expect = len - pos;
            if (expect == flush()) {
                memcpy(this->buffer, (char *) buf, length);
                this->len = length;
                expect = length;
                return static_cast<int64_t>(expect);
            } else {
                // Failed to flush
                return -1;
            }
        }
    } else {
        if (this->len != this->pos) {
            // There is a surplus in the buffer and needs to be flush
            size_t expect = len - pos;
            if (expect != flush()) {
                // Failed to flush
                return -1;
            }
        }

        int64_t wrote = 0;
        while (true) {
            auto rt = source->write(static_cast<const char *>(buf) + wrote, length - wrote >= cap ? cap : length - wrote);
            if (rt <= 0) return -1;
            wrote += rt;
            if (wrote == length) break;
        }

        return wrote;
    }
}

int64_t BufferedOutputStream::flush() noexcept {
    // Write existing unprocessed data to the stream immediately
    auto wrote = source->write(static_cast<char *>(buffer) + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}