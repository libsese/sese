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

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>

#include <cassert>

namespace sese::text {

namespace overload {

    template<typename VALUE>
    struct Formatter {
        std::string format(const VALUE &value) {
            return std::to_string(value);
        }
    };

    template<>
    struct Formatter<std::string> {
        static std::string format(const std::string &value) {
            return value;
        }
    };

    template<>
    struct Formatter<const char*> {
        static std::string format(const char *value) {
            return { value };
        }
    };

} // namespace overload

struct FmtCtx {
    StringBuilder builder;
    std::string_view pattern;
    std::string_view::const_iterator pos;

    explicit FmtCtx(std::string_view p);

    bool constantParsing();
};

template<typename T>
void Format(FmtCtx &ctx, T &&arg) {
    auto status = ctx.constantParsing();
    if (status) {
        auto formatter = overload::Formatter<std::decay_t<T>>();
        ctx.builder << formatter.format(std::forward<T>(arg));
        [[maybe_unused]] auto result = ctx.constantParsing();
        assert(false == result);
    }
}

template<typename T, typename... ARGS>
void Format(FmtCtx &ctx, T &&arg, ARGS&&... args) {
    auto status = ctx.constantParsing();
    if (status) {
        auto formatter = overload::Formatter<std::decay_t<T>>();
        ctx.builder << formatter.format(std::forward<T>(arg));
        Format(ctx, std::forward<ARGS>(args)...);
    }
}

/// 字符串格式化
/// \tparam ARGS 形参
/// \param pattern 匹配格式
/// \return 匹配完成的字符串
template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) == 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS&&...) {
    return { pattern.begin(), pattern.end() };
}

/// 字符串格式化
/// \tparam ARGS 形参
/// \param pattern 匹配格式
/// \param args 实参
/// \return 匹配完成的字符串
template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) != 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS&&... args) {
    FmtCtx ctx(pattern);
    Format(ctx, std::forward<ARGS>(args)...);
    return ctx.builder.toString();
}

} // namespace sese::text