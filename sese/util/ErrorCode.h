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
/// @brief Error code class
/// @author: kaoru
/// @date November 1, 2024

#pragma once

#include <string>

namespace sese {

/// @brief Error code class
class ErrorCode {
    int32_t error;
    std::string msg;

public:
    /// @brief Constructor
    /// @param error Error code
    /// @param msg Error description
    ErrorCode(int32_t error, std::string msg) noexcept;

    /// @brief Get error code
    /// @return Error code
    [[nodiscard]] int32_t value() const noexcept { return error; }

    /// @brief Get error description
    /// @return Error description
    [[nodiscard]] const std::string &message() const noexcept { return msg; }

    explicit operator bool() const noexcept {
        return value() != 0;
    }
};
} // namespace sese