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
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UNICODE
 * @author kaoru
 * @date 2022年4月15日
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
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UTF-8
 * @deprecated 请考虑使用 sese::text::TextReader
 */
class  UniReader final {
public:
    using Ptr = std::shared_ptr<UniReader>;

    ~UniReader();

    /**
     * 打开文件
     * @param fileName 文件名称
     * @retval nullptr 打开失败
     */
    static std::shared_ptr<UniReader> create(const std::string &fileName) noexcept;

    /**
     * 读取一行
     * @return 读取到的文本，当 string.empty() 为 true 时表示没有内容可供再读取
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