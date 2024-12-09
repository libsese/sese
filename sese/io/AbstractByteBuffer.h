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
 * @file AbstractByteBuffer.h
 * @brief Byte buffer class
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/io/Stream.h"
#include "sese/io/PeekableStream.h"

namespace sese::io {

/**
 * @brief Byte buffer class
 */
class  AbstractByteBuffer : public Stream, public PeekableStream {
private:
    /**
     * @brief Buffer node
     */
    struct Node {
        /// Buffer memory
        void *buffer = nullptr;
        /// Next node
        Node *next = nullptr;
        /// Memory used by the node
        size_t length = 0;
        /// Capacity of the node
        size_t cap = 0;
        /**
         * Initialize node
         * @param buffer_size Size of memory allocated for the node
         */
        explicit Node(size_t buffer_size);
        /// Destructor
        ~Node();
    };

public:
    /**
     * @param base_size Initial node memory size
     * @param factor Size of additional memory nodes
     */
    explicit AbstractByteBuffer(size_t base_size = STREAM_BYTE_STREAM_SIZE_FACTOR, size_t factor = STREAM_BYTE_STREAM_SIZE_FACTOR);

    /// Copy
    AbstractByteBuffer(const AbstractByteBuffer &abstract_byte_buffer) noexcept;
    /// Mobile semantics
    AbstractByteBuffer(AbstractByteBuffer &&abstract_byte_buffer) noexcept;

    ~AbstractByteBuffer() override;
    /// Reset the read position
    virtual void resetPos();
    /// Whether there is readable data
    virtual bool eof();

    /**
     * @return Total memory used by all nodes
     */
    [[nodiscard]] virtual size_t getLength() const;
    /**
     * @return Total capacity of all nodes
     */
    [[nodiscard]] virtual size_t getCapacity() const;
    /**
     * @return Total readable bytes
     */
    [[nodiscard]] virtual size_t getReadableSize() const;

    /**
     * Release all nodes before CurrentReadNode
     * @return Actual released space in bytes
     */
    virtual size_t freeCapacity();

    virtual void swap(AbstractByteBuffer &other) noexcept;

    // [[nodiscard]] size_t getCurrentWritePos() const { return currentWritePos; }
    // [[nodiscard]] size_t getCurrentReadPos() const { return currentReadPos; }

public:
    int64_t read(void *buffer, size_t len) override;
    int64_t write(const void *buffer, size_t len) override;

    int64_t peek(void *buffer, size_t len) override;
    int64_t trunc(size_t need_read) override;

private:
    size_t factor = 0;

    Node *root = nullptr;
    Node *currentWriteNode = nullptr;
    size_t currentWritePos = 0;
    Node *currentReadNode = nullptr;
    size_t currentReadPos = 0;

    /// length does not count the last node, the real length should be:
    /// realLength = length + currentWriteNode->length
    size_t length = 0;
    size_t cap = 0;
};

} // namespace sese::io