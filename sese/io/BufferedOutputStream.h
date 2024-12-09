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

/// \file BufferedOutputStream.h
/// \brief Buffered output stream class
/// \version 0.1
/// \author kaoru
/// \date November 20, 2022

#pragma once

#include "sese/Config.h"
#include "sese/io/OutputStream.h"

namespace sese::io {

/// Buffered output stream class
class BufferedOutputStream : public OutputStream {
public:
    using Ptr = std::shared_ptr<BufferedOutputStream>;

    explicit BufferedOutputStream(const OutputStream::Ptr &source, size_t buffer_size = STREAM_BYTE_STREAM_SIZE_FACTOR);

    ~BufferedOutputStream() noexcept override;

    int64_t write(const void *buffer, size_t length) override;

    int64_t flush() noexcept;

    [[nodiscard]] size_t getPosition() const { return pos; }
    [[nodiscard]] size_t getLength() const { return len; }
    [[nodiscard]] size_t getCapacity() const { return cap; }

private:
    OutputStream::Ptr source;
    void *buffer = nullptr;
    size_t pos;
    size_t len;
    size_t cap;
};
} // namespace sese::io