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
 * \file InputBufferWrapper.h
 * \author kaoru
 * \date 2022.12.14
 * \version 0.1
 * \brief 输入缓存包装器
 */
#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"
#include "sese/io/PeekableStream.h"

namespace sese::io {

// GCOVR_EXCL_START

/// \brief 输入缓存包装器
class  InputBufferWrapper final : public InputStream, public PeekableStream {
public:
    InputBufferWrapper(const char *buffer, size_t cap);

    int64_t read(void *buffer, size_t length) override;

    int64_t peek(void *buffer, size_t length) override;

    int64_t trunc(size_t length) override;

    void reset() noexcept;

    [[nodiscard]] const char *getBuffer() const;
    [[nodiscard]] size_t getLength() const;
    [[nodiscard]] size_t getCapacity() const;

protected:
    const char *buffer = nullptr;
    size_t pos = 0;
    size_t cap = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io

int64_t operator<<(sese::io::OutputStream &out, sese::io::InputBufferWrapper &input) noexcept;

int64_t operator<<(sese::io::OutputStream *out, sese::io::InputBufferWrapper &input) noexcept;