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
 * @file IOBuf.h
 * @brief Chained buffer for IOCP
 * @author kaoru
 * @date September 20, 2023
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/InputStream.h>
#include <sese/io/PeekableStream.h>

#include <list>

namespace sese::iocp {

/// IOBuf Node
struct IOBufNode {
    /// Cache pointers
    void *buffer{nullptr};
    /// Cache read size
    size_t read{0};
    /// The cache is populated with size
    size_t size{0};
    /// The size of the cache capacity
    const size_t CAPACITY;

    /**
     * Creates a new node
     * @param capacity The size of the node's capacity
     */
    explicit IOBufNode(size_t capacity);

    ~IOBufNode();

    /**
     * Get the readable size of the node
     * @return Readable size
     */
    [[nodiscard]] size_t getReadableSize() const noexcept;

    /**
     * Get the writable size of the node
     * @return Writable size (free portion)
     */
    [[nodiscard]] size_t getWriteableSize() const noexcept;
};

/// Chained buffer for IOCP
class IOBuf final : public io::InputStream, public io::PeekableStream {
public:
    /// Type of node
    using Node = std::unique_ptr<IOBufNode>;
    using ListType = std::list<Node>;

    /// Add a new node. Once the node is added, it can no longer be modified externally
    /// \param node Target node
    void push(Node node);

    /// Release all nodes
    void clear();

    /// Get the current readable size
    [[nodiscard]] size_t getReadableSize() const noexcept;

    /// Get the current total size of all node contents
    [[nodiscard]] size_t getTotalSize() const noexcept;

    /// Read content from one or more nodes
    /// \param buffer Buffer
    /// \param length Buffer size
    /// \return Actual size obtained
    int64_t read(void *buffer, size_t length) override;

    /// Peek at the content from one or more nodes
    /// \param buffer Buffer
    /// \param length Buffer size
    /// \return Actual size obtained
    int64_t peek(void *buffer, size_t length) override;

    /// Step through the content from one or more nodes
    /// \param length Step size
    /// \return Actual step size
    int64_t trunc(size_t length) override;

private:
    ListType list;
    ListType::iterator cur;

    size_t total{0};
    size_t readed{0};
};

} // namespace sese::iocp
