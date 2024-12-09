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
 * @file BufferedStream.h
 * @author kaoru
 * @date May 24, 2022
 * @brief Stream buffer class
 */

#pragma once

#include "sese/io/Stream.h"
#include "sese/util/Noncopyable.h"

namespace sese::io {

/// @brief Stream buffer class
/// @deprecated Consider using BufferedOutputStream or BufferedInputStream
class  BufferedStream final : public Noncopyable, public Stream {
public:
    using Ptr = std::shared_ptr<BufferedStream>;

    /**
     * Wrap the specified stream
     * @param source Stream to be wrapped
     * @param buffer_size Buffer size
     */
    explicit BufferedStream(const Stream::Ptr &source, size_t buffer_size = STREAM_BYTE_STREAM_SIZE_FACTOR);
    ~BufferedStream() noexcept override;

    /**
     * @verbatim
     * Read a certain number of bytes
     * If the read size does not exceed the BufferedStream buffer size, the read will be automatically cached
     * Otherwise, direct raw stream reading is performed to reduce copy operations
     * @endverbatim
     * @param buffer User buffer pointer
     * @param length User buffer size
     * @return Actual number of bytes read
     */
    int64_t read(void *buffer, size_t length) override;

    /**
     * @brief Clear the current read flag
     * @warning Ensure that all content in BufferedStream has been read out before performing this operation, otherwise it may result in the loss of necessary information
     */
    void clear() noexcept;

    /**
     * @verbatim
     * Write a certain number of bytes
     * If the write size does not exceed the BufferedStream buffer size, the write will be automatically flushed
     * Otherwise, direct raw stream writing is performed to reduce copy operations
     * @endverbatim
     * @param buffer User buffer pointer
     * @param length User buffer size
     * @return Actual number of bytes written
     */
    int64_t write(const void *buffer, size_t length) override;

    /**
     * @brief Flush all current buffer contents to the stream and reset the current write flag
     * @return Number of bytes flushed to the stream
     */
    int64_t flush() noexcept;

    /// Reset output source; this will cause existing data to be cleared
    /// \param source New output source
    void reset(const Stream::Ptr &source) noexcept;

    [[nodiscard]] size_t getPosition() const { return pos; }
    [[nodiscard]] size_t getLength() const { return len; }
    [[nodiscard]] size_t getCapacity() const { return cap; }

private:
    inline int64_t preRead();

    /// Source stream pointer
    Stream::Ptr source;
    /// Cache pointer
    void *buffer = nullptr;
    /// Size of processed data
    size_t pos;
    /// Size of data available for operation
    size_t len;
    /// Cache capacity
    size_t cap;
};
} // namespace sese::io