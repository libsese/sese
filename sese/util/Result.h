/// @file Result.h
/// @brief 结果类
/// @author: kaoru
/// @date 2024年11月1日

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

#include "sese/net/Socket.h"
#include "Util.h"
#include "ErrorCode.h"

#include <variant>

namespace sese {

/// @brief 结果类
/// @tparam T 结果类型
template<class T>
class Result final {
    std::variant<ErrorCode, T> err_result;

public:
    /// @brief 构造函数
    /// @param err 错误码
    Result(ErrorCode err) : err_result(err) {} // NOLINT

    /// @brief 构造函数
    /// @param code 错误代码
    /// @param msg 错误描述
    Result(int32_t code, const std::string &msg) : err_result(ErrorCode{code, msg}) {}

    /// @brief 构造函数
    /// @param result 结果
    Result(T result) : err_result(std::forward<T>(result)) {} // NOLINT

    static Result fromLastError();

    static Result fromLastNetworkError();

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return std::holds_alternative<ErrorCode>(err_result);
    }

    /// @brief 获取错误码
    /// @return 错误码
    [[nodiscard]] ErrorCode err() const noexcept {
        assert(std::holds_alternative<ErrorCode>(err_result));
        return std::get<ErrorCode>(err_result);
    }

    /// @brief 获取结果
    /// @return 结果
    T &get() noexcept {
        assert(std::holds_alternative<T>(err_result));
        return std::get<T>(err_result);
    }

    /// @brief 获取结果
    /// @return 结果
    const T &get() const noexcept {
        assert(std::holds_alternative<T>(err_result));
        return std::get<T>(err_result);
    }
};

template<class T>
Result<T> Result<T>::fromLastError() {
    return {getErrorCode(), getErrorString()};
}

template<class T>
Result<T> Result<T>::fromLastNetworkError() {
    return {net::getNetworkError(), net::getNetworkErrorString()};
}

} // namespace sese