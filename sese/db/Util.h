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

/// \file Util.h
/// \brief 工具集合
/// \author kaoru
/// \version 0.1

#pragma once

#include <map>
#include <string>

namespace sese::db {

    /// \brief 对字符串进行简单的分词
    /// \param string 目标字符串
    /// \return 分词结果
    std::map<std::string, std::string> tokenize(const char *string) noexcept;
}