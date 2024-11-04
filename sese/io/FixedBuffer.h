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

/// \file FixedBuffer.h
/// \brief 线程安全的固定大小缓存
/// \author kaoru
/// \version 0.1.0
/// \date 2023年6月11日

#pragma once

#include "sese/io/AbstractFixedBuffer.h"

#include <mutex>

namespace sese::io {

/// 线程安全的固定大小缓存
class  FixedBuffer final : public AbstractFixedBuffer {
public:
    explicit FixedBuffer(size_t size) noexcept;

    int64_t read(void *buffer, size_t length) override;

    int64_t write(const void *buffer, size_t length) override;

    int64_t peek(void *buffer, size_t length) override;

    int64_t trunc(size_t length) override;

    void reset() noexcept override;

    size_t getReadableSize() noexcept;

    [[nodiscard]] size_t getWriteableSize() noexcept;

private:
    std::mutex mutex;
};

} // namespace sese::io