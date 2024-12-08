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
 * @file TextReader.h
 * @author kaoru
 * @version 0.1
 * @brief Text reader class
 * @date September 13, 2023
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/FileStream.h>
#include <sese/io/BufferedStream.h>
#include <sese/text/String.h>

namespace sese::text {

/// Text reader class
class TextReader final {
public:
    using Ptr = std::shared_ptr<TextReader>;

    ~TextReader() noexcept;
    static std::shared_ptr<sese::text::TextReader> create(const char *u8str) noexcept;
    String readLine();

private:
    TextReader() = default;

    io::FileStream::Ptr fileStream;
    io::BufferedStream::Ptr bufferedStream;
};
} // namespace sese::text