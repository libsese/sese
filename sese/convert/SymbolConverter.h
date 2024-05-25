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

#pragma once

#include <string>
#include <sese/util/NotInstantiable.h>

namespace sese {

class SymbolConverter : public NotInstantiable {
public:
    /// 解析 typeid 生成的符号字面值
    /// @warning 只支持类信息
    /// @param type GNU typeid 操作符结果
    /// @return 实际符号的字面值
    static std::string decodeGNUClassName(const std::type_info *type) noexcept;

    /// 解析 typeid 生成的符号字面值
    /// @warning 只支持类信息
    /// @param type MSVC typeid 操作符结果
    /// @return 实际符号的字面值
    static std::string decodeMSVCClassName(const std::type_info *type) noexcept;
};

} // namespace sese