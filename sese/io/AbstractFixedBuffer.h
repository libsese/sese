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
 * @file AbstractFixedBuffer.h
 * @brief 固定大小的字节缓冲区类
 * @author kaoru
 * @date 2023年9月13日
 */

#pragma once

#include "sese/io/Stream.h"
#include "sese/io/PeekableStream.h"

namespace sese::io {

/// 固定大小的字节缓冲区类
class  AbstractFixedBuffer : public Stream, public PeekableStream {
public:
    explicit AbstractFixedBuffer(size_t size) noexcept;
    ~AbstractFixedBuffer() noexcept override;
    // copy
    AbstractFixedBuffer(const AbstractFixedBuffer &buffer) noexcept;
    // move
    AbstractFixedBuffer(AbstractFixedBuffer &&buffer) noexcept;

    int64_t read(void *buffer, size_t length) override;
    int64_t write(const void *buffer, size_t length) override;
    int64_t peek(void *buffer, size_t length) override;
    int64_t trunc(size_t length) override;
    virtual void reset() noexcept;

    [[nodiscard]] const char *data() const { return buffer; }
    [[nodiscard]] size_t getSize() const noexcept { return size; }
    [[nodiscard]] size_t getReadSize() const noexcept { return readSize; }
    [[nodiscard]] size_t getWriteSize() const noexcept { return writeSize; }
    [[nodiscard]] size_t getReadableSize() const noexcept { return writeSize - readSize; }
    [[nodiscard]] size_t getWriteableSize() const noexcept { return size - writeSize; }

protected:
    size_t size{};
    size_t readSize{};
    size_t writeSize{};
    char *buffer{};
};

} // namespace sese::io