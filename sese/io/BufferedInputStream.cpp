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

#include "sese/io/BufferedInputStream.h"

using sese::io::BufferedInputStream;

BufferedInputStream::BufferedInputStream(const InputStream::Ptr &source, size_t buffer_size) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = buffer_size;
    this->buffer = malloc(cap);
}

BufferedInputStream::~BufferedInputStream() noexcept {
    free(buffer);
}

inline int64_t BufferedInputStream::preRead() noexcept {
    // Try populating the cache with the target stream
    auto read = source->read(buffer, cap);
    // This is used to fix cases where some input sources may read and return negative values
    read = read < 0 ? 0 : read; // GCOVR_EXCL_LINE
    pos = 0;
    len = read;
    return read;
}

int64_t BufferedInputStream::read(void *buf, size_t length) {
    /*
     * If the number of bytes required to read needs caching less than twice,
     * a pre-read operation is required to avoid frequent I/O operations;
     * If the number of bytes required to read needs caching two times or more,
     * then after processing the original cache, operate directly on the raw stream to reduce the number of copies
     */
    if (length <= this->cap) {
        if (this->len - this->pos >= length) {
            // The number of bytes is sufficient - no pre-reads are required
            memcpy(buf, static_cast<char *>(this->buffer) + this->pos, length);
            pos += length;
            return static_cast<int64_t>(length);
        } else {
            // Insufficient bytes - Pre-read is required
            size_t total = this->len - this->pos;
            memcpy(buf, static_cast<char *>(this->buffer) + this->pos, total);
            pos += total;
            if (0 != preRead()) {
                if (this->len - this->pos >= length - total) {
                    // The number of bytes is sufficient
                    memcpy(static_cast<char *>(buf) + total, this->buffer, length - total);
                    pos = length - total;
                    total = length;
                } else {
                    // The number of bytes is insufficient, and it cannot be read further
                    memcpy(static_cast<char *>(buf) + total, this->buffer, this->len - this->pos);
                    pos = this->len - this->pos;
                    total += this->len - this->pos;
                }
            }
            return static_cast<int64_t>(total);
        }
    } else {
        // Deal with the existing cache first
        size_t total = this->len - this->pos;
        memcpy(buf, this->buffer, total);
        this->len = 0;
        this->pos = 0;
        while (true) {
            size_t read = source->read(static_cast<char *>(buf) + total, (length - total) >= 1024 ? 1024 : length - total);
            total += static_cast<int64_t>(read);
            if (read <= 0) break;
            if (total == length) break;
        }
        return static_cast<int64_t>(total);
    }
}