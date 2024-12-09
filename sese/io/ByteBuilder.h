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
 * @file ByteBuilder.h
 * @brief Non-thread-safe byte buffer class
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/AbstractByteBuffer.h"

namespace sese::io {

/**
 * @brief Non-thread-safe byte buffer class
 */
class  ByteBuilder final : public AbstractByteBuffer {
public:
    using Ptr = std::unique_ptr<ByteBuilder>;

    ByteBuilder(size_t baseSize = STREAM_BYTE_STREAM_SIZE_FACTOR, size_t factor = STREAM_BYTE_STREAM_SIZE_FACTOR) noexcept // NOLINT
        : AbstractByteBuffer(baseSize, factor) {
    }
};

} // namespace sese::io