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

/// \file BlockAppender.h
/// \author kaoru
/// \date August 6, 2022
/// \brief Block file log appender class
/// \version 0.1

#pragma once

#include "sese/log/AbstractAppender.h"
#include "sese/io/FileStream.h"
#include "sese/io/BufferedStream.h"

#include <mutex>

namespace sese::log {

/// Block file log appender class
class BlockAppender final : public AbstractAppender {
public:
    /// Initialize block parameters
    /// \param block_max_size Preset size of the block. This parameter has a minimum value limit in non-DEBUG mode, at least 1000 * 1024, which is 1MB
    /// \param level Threshold level
    explicit BlockAppender(size_t block_max_size, Level level = Level::DEBUG);

    ~BlockAppender() noexcept override;

    void dump(const char *buffer, size_t size) noexcept override;

#if defined(SESE_BUILD_TEST)
public:
#else
private:
#endif
    size_t size = 0;
    std::mutex mutex;
    size_t maxSize = 0;
    io::BufferedStream::Ptr bufferedStream;
    io::FileStream::Ptr fileStream;
};

} // namespace sese::log