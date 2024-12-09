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

/**
 * @file ByteBuffer.h
 * @brief Thread-safe byte buffer class
 * @author kaoru
 * @date April 5, 2022
 */

#pragma once

#include "sese/io/AbstractByteBuffer.h"
#include <mutex>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/**
 * @brief Thread-safe byte buffer class
 */
class  ByteBuffer final : AbstractByteBuffer {
public:
    using Ptr = std::unique_ptr<ByteBuffer>;

    explicit ByteBuffer(size_t base_size = STREAM_BYTE_STREAM_SIZE_FACTOR, size_t factor = STREAM_BYTE_STREAM_SIZE_FACTOR);
    void resetPos() override;
    bool eof() override;
    [[nodiscard]] size_t getLength();
    [[nodiscard]] size_t getCapacity();
    size_t freeCapacity() override;
    int64_t read(void *buffer, size_t len) override;
    int64_t write(const void *buffer, size_t len) override;
    int64_t peek(void *buffer, size_t len) override;
    int64_t trunc(size_t need_read) override;
    // [[nodiscard]] size_t getCurrentWritePos();
    // [[nodiscard]] size_t getCurrentReadPos();

private:
    using AbstractByteBuffer::getCapacity;
    using AbstractByteBuffer::getLength;

    std::mutex mutex;
};
} // namespace sese::io