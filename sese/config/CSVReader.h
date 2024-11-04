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

/// \file CSVReader.h
/// \brief CSV 流读取器
/// \author kaoru
/// \version 0.1
/// \date 2023年3月30日

#pragma once

#include "sese/io/InputStream.h"

#include <vector>
#include <string>

namespace sese {

/// CSV 流读取器
class  CSVReader {
public:
    /// 工具支持的输入流
    using InputStream = io::InputStream;
    /// 行
    using Row = std::vector<std::string>;

    /// 构造函数
    /// \param source 读取的流
    /// \param split_char 分割字符
    explicit CSVReader(InputStream *source, char split_char = ',') noexcept;

    /// 从流中读取一行
    /// \return 返回包含一行内所有元素的 vector 容器
    Row read() noexcept;

protected:
    char splitChar;
    InputStream *source = nullptr;
};
} // namespace sese
