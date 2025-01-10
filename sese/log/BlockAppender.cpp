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

#include "sese/log/BlockAppender.h"
#include "sese/text/DateTimeFormatter.h"
#include "sese/thread/Locker.h"

#include <algorithm>

using namespace sese;
using namespace sese::log;

inline std::string getDateTimeString() {
    auto date_time = DateTime::now();
    return text::DateTimeFormatter::format(date_time, RECORD_DEFAULT_FILE_TIME_PATTERN) + ".log";
}

BlockAppender::BlockAppender(size_t block_max_size, sese::log::Level level)
    : AbstractAppender(level), maxSize(block_max_size) {
    // #ifndef _DEBUG
    //     maxSize = blockMaxSize < 1000 * 1024 ? 1000 * 1024 : blockMaxSize;
    // #endif
    maxSize = std::max<decltype(maxSize)>(1024 * 1000, maxSize);
    auto file_name = getDateTimeString();
    fileStream = io::FileStream::create(file_name, io::File::T_WRITE_TRUNC);
    bufferedStream = std::make_unique<io::BufferedStream>(fileStream, 4 * 1024);
}

BlockAppender::~BlockAppender() noexcept {
    bufferedStream->flush();
    fileStream->close();
}

void BlockAppender::dump(const char *buffer, size_t i) noexcept {
    Locker locker(this->mutex);
    if (this->size + i > this->maxSize) {
        this->size = i;
        bufferedStream->flush();
        fileStream->close();
        auto file_name = getDateTimeString();
        fileStream = io::FileStream::create(file_name, io::File::T_WRITE_TRUNC);
        bufferedStream->reset(fileStream);
        bufferedStream->write((void *) buffer, i);
    } else {
        this->size += i;
        bufferedStream->write((void *) buffer, i);
    }
}
