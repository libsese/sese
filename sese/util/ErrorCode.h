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

/// @file ErrorCode.h
/// @brief 错误码类
/// @author: kaoru
/// @date 2024年11月1日

#pragma once

#include <string>

namespace sese {

/// @brief 错误码
class ErrorCode {
    int32_t error;
    std::string msg;

public:
    /// @brief 构造函数
    /// @param error 错误码
    /// @param msg 错误描述
    ErrorCode(int32_t error, std::string msg) noexcept;

    /// @brief 获取错误码
    /// @return 错误码
    [[nodiscard]] int32_t value() const noexcept { return error; }

    /// @brief 获取错误描述
    /// @return 错误描述
    [[nodiscard]] const std::string &message() const noexcept { return msg; }

    explicit operator bool() const noexcept {
        return value() != 0;
    }
};
} // namespace sese