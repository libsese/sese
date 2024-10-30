/// @file ErrorCategoryWrapper.h
/// @brief 错误类别包装器
/// @author kaoru
/// @date 2024年10月30日

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

#include <system_error>

namespace sese::system {

/// \brief 错误类别包装器
/// \warning 此包装器纯粹是为了自定义 message，暂时不推荐使用
class ErrorCategoryWrapper final : public std::error_category {
    std::string message_;

public:
    explicit ErrorCategoryWrapper(const std::string &message);

    [[nodiscard]] const char *name() const noexcept override;
    [[nodiscard]] std::string message(int code = 0) const override;
};

} // namespace sese::system