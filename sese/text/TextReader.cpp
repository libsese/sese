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

#include <sese/text/TextReader.h>

namespace sese::text {

TextReader::~TextReader() noexcept {
    fileStream->close();
}

std::shared_ptr<sese::text::TextReader> TextReader::create(const char *u8str) noexcept {
    auto file_stream = io::FileStream::create(u8str, io::File::T_READ);
    if (file_stream == nullptr) {
        return nullptr;
    }

    auto buffered_stream = std::make_shared<io::BufferedStream>(file_stream);
    auto reader = MAKE_SHARED_PRIVATE(TextReader);
    reader->fileStream = file_stream;
    reader->bufferedStream = buffered_stream;
    return reader;
}

String TextReader::readLine() {
    size_t i = 0;
    char buffer[1024];
    while (i < 1020) {
        auto success = bufferedStream->read(buffer + i, 1);
        if (1 == success) {
            if ('\r' == buffer[i] || '\n' == buffer[i]) {
                if (0 == i) {
                    continue;
                } else {
                    buffer[i] = 0;
                    return {buffer, i};
                }
            }

            auto size = sstr::getSizeFromUTF8Char(*(buffer + i));

            if (size > 1) {
                bufferedStream->read(buffer + i + 1, size - 1);
            }
            i += size;
        } else {
            if (0 == i) {
                return String();
            } else {
                buffer[i] = 0;
                return {buffer, i};
            }
        }
    }
    return {buffer, i};
}
} // namespace sese::text