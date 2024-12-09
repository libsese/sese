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

#include "sese/io/BufferedStream.h"
#include <cstring>

using sese::io::BufferedStream;

BufferedStream::BufferedStream(const Stream::Ptr &source, size_t buffer_size) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = buffer_size;
    this->buffer = malloc(cap);
}

BufferedStream::~BufferedStream() noexcept {
    free(buffer);
}

inline int64_t BufferedStream::preRead() {
    // Try populating the cache with the target stream
    auto read = source->read(buffer, cap);
    // This is used to fix cases where some input sources may read and return negative values
    read = read < 0 ? 0 : read; // GCOVR_EXCL_LINE
    pos = 0;
    len = read;
    return read;
}

void BufferedStream::clear() noexcept {
    // Clear all flags
    pos = 0;
    len = 0;
}

int64_t BufferedStream::read(void *buf, size_t length) {
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
            // No more reading
            if (read <= 0) break;
            // Accomplish the goal
            if (total == length) break;
        }
        return static_cast<int64_t>(total);
    }
}

int64_t BufferedStream::write(const void *buf, size_t length) {
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
        // Write directly
        if (this->len != this->pos) {
            // There is a surplus in the buffer and needs to be refreshed
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

int64_t BufferedStream::flush() noexcept {
    // Write existing unprocessed data to the stream immediately
    auto wrote = source->write(static_cast<char *>(buffer) + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}

void BufferedStream::reset(const sese::io::Stream::Ptr &new_source) noexcept {
    this->source = new_source;
    pos = 0;
    len = 0;
}
