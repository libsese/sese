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

/// @file Result.h
/// @brief 结果类
/// @author kaoru
/// @date 2024年11月28日

#pragma once

#include <cassert>
#include <optional>
#include <type_traits>
#include <variant>

namespace sese {

/// @brief 结果类
/// @tparam T 结果类型
/// @tparam E 错误类型
template<class T, class E, class Enable = void>
class Result;

template<class T, class E>
class Result<T, E, std::enable_if_t<std::is_same_v<T, E> && !std::is_void_v<T> && !std::is_void_v<E>>> {
private:
    bool is_success;
    T result;

    Result() = default;

public:
    using ResultType = T;
    using ErrorType = E;

    static Result error(E error) noexcept {
        Result<T, E> tmp;
        tmp.is_success = false;
        tmp.result = error;
        return tmp;
    }

    static Result success(T result) noexcept {
        Result<T, E> tmp;
        tmp.is_success = true;
        tmp.result = result;
        return tmp;
    }

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return is_success;
    }

    /// @brief 获取错误
    /// @return 错误
    E err() const noexcept {
        assert(!is_success);
        return result;
    }

    /// @brief 获取错误
    /// @return 错误
    E &err() noexcept {
        assert(!is_success);
        return result;
    }

    /// @brief 获取结果
    /// @return 结果
    T get() const noexcept {
        assert(is_success);
        return result;
    }

    /// @brief 获取结果
    /// @return 结果
    T &get() noexcept {
        assert(is_success);
        return result;
    }
};

template<class T, class E>
class Result<T, E, std::enable_if_t<!std::is_same_v<T, E> && !std::is_void_v<T> && !std::is_void_v<E>>> final {
private:
    std::variant<T, E> result;

public:
    using ResultType = T;
    using ErrorType = E;

    static Result error(E error) noexcept {
        Result<T, E> tmp;
        tmp.result = std::forward<E>(error);
        return tmp;
    }

    static Result success(T result) noexcept {
        Result<T, E> tmp;
        tmp.result = std::forward<T>(result);
        return tmp;
    }

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return std::holds_alternative<E>(result);
    }

    /// @brief 获取错误
    /// @return 错误
    [[nodiscard]] E &err() noexcept {
        assert(std::holds_alternative<E>(result));
        return std::get<E>(result);
    }

    /// @brief 获取错误
    /// @return 错误
    E err() const noexcept {
        assert(std::holds_alternative<E>(result));
        return std::get<E>(result);
    }

    /// @brief 获取结果
    /// @return 结果
    T &get() noexcept {
        assert(std::holds_alternative<T>(result));
        return std::get<T>(result);
    }

    /// @brief 获取结果
    /// @return 结果
    T get() const noexcept {
        assert(std::holds_alternative<T>(result));
        return std::get<T>(result);
    }
};

template<class T>
class Result<T, void> {
private:
    std::optional<T> result;

    Result() = default;

public:
    using ResultType = T;
    using ErrorType = void;

    static Result error() noexcept {
        return {};
    }

    static Result success(T result) noexcept {
        Result tmp;
        tmp.result = std::forward<T>(result);
        return tmp;
    }

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return result.has_value();
    }

    /// @brief 获取结果
    /// @return 结果
    T &get() noexcept {
        assert(result.has_value());
        return result.value();
    }

    /// @brief 获取结果
    /// @return 结果
    T get() const noexcept {
        assert(result.has_value());
        return result.value();
    }
};

template<class E>
class Result<void, E> {
private:
    std::optional<E> e;

    Result() = default;

public:
    using ResultType = void;
    using ErrorType = E;

    static Result error(E error) noexcept {
        Result tmp;
        tmp.e = std::forward<E>(error);
        return tmp;
    }

    static Result success() noexcept {
        return {};
    }

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return !e.has_value();
    }

    /// @brief 获取错误
    /// @return 错误
    [[nodiscard]] E &err() noexcept {
        assert(e.has_value());
        return e.value();
    }

    /// @brief 获取错误
    /// @return 错误
    E err() const noexcept {
        assert(e.has_value);
        return e.value();
    }
};

template<>
class Result<void, void> {
private:
    bool is_success;

    Result() = default;

public:
    using ResultType = void;
    using ErrorType = void;

    static Result error() noexcept {
        Result tmp;
        tmp.is_success = false;
        return tmp;
    }

    static Result success() noexcept {
        Result tmp;
        tmp.is_success = true;
        return tmp;
    }

    /// @brief 判断是否有错误
    /// @return 是否有错误
    explicit operator bool() const noexcept {
        return is_success;
    }
};

} // namespace sese