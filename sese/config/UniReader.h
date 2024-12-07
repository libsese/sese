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
 * @file UniReader.h
 * @brief Cross-platform file reader, primarily adapted for Windows, supports UNICODE
 * @author Kaoru
 * @date April 15, 2022
 */

#pragma once

#include "sese/io/BaseStreamReader.h"
#include "sese/io/FileStream.h"

#ifdef SESE_PLATFORM_WINDOWS
#include <codecvt>
#ifdef __MINGW64__
#include <locale>
#endif
#endif

namespace sese {

/**
 * @brief Cross-platform file reader, primarily adapted for Windows, supports UTF-8
 * @deprecated Please consider using sese::text::TextReader
 */
class  UniReader final {
public:
    using Ptr = std::shared_ptr<UniReader>;

    ~UniReader();

    /**
     * Opens a file
     * @param fileName The file name
     * @retval nullptr If opening fails
     */
    static std::shared_ptr<UniReader> create(const std::string &fileName) noexcept;

    /**
     * Reads a line
     * @return The text read. When string.empty() is true, it indicates no more content to read
     */
    std::string readLine();

private:
    UniReader() = default;

    io::FileStream::Ptr fileStream;
#ifdef SESE_PLATFORM_WINDOWS
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    io::WStreamReader::Ptr reader;
#else
    io::StreamReader::Ptr reader;
#endif
};
} // namespace sese