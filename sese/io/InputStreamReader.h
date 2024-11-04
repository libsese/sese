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

/// \file InputStreamReader.h
/// \author kaoru
/// \brief 可读流读取器
/// \date 2023年9月11日

#pragma once

#include <sese/io/PeekableStream.h>
#include <sese/text/StringBuilder.h>

namespace sese::io {

/// 可读流读取器
class  InputStreamReader {
public:
    explicit InputStreamReader(io::PeekableStream *input) noexcept;

    /// 读取一行
    /// \note 支持 CR、LF、CRLF 换行分隔符
    /// \return 如果内容全部读完（例如文件流的 EOF），则返回 std::string {}
    std::string readLine() noexcept;

protected:
    PeekableStream *input;
    text::StringBuilder builder;
};
} // namespace sese::io