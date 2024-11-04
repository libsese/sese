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

#include "sese/record/FileAppender.h"

// GCOVR_EXCL_START

sese::record::FileAppender::FileAppender(io::FileStream::Ptr file_stream, sese::record::Level level)
    : AbstractAppender(level),
      fileStream(std::move(file_stream)) {
    bufferedStream = std::make_shared<io::BufferedStream>(this->fileStream, 4 * 1024);
}

// GCOVR_EXCL_STOP

sese::record::FileAppender::~FileAppender() noexcept {
    bufferedStream->flush();
    fileStream->close();
}

void sese::record::FileAppender::dump(const char *buffer, size_t size) noexcept {
    bufferedStream->write((void *) buffer, size);
}
