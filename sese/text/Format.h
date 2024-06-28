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
        void parse(const std::string &) {}
        std::string format(const VALUE &value) {
            return std::to_string(value);
        }
    };

    template<>
    struct Formatter<std::string> {
        void parse(const std::string &) {}
        static std::string format(const std::string &value) {
            return value;
        }
    };

    template<>
    struct Formatter<const char *> {
        void parse(const std::string &) {}
        static std::string format(const char *value) {
            return {value};
        }
    };

} // namespace overload

struct FmtCtx {
    StringBuilder builder;
    std::string_view pattern;
    std::string_view::const_iterator pos;

    explicit FmtCtx(std::string_view p) : pattern(p), pos(pattern.begin()) {
    }

    bool constantParsing(std::string &args) {
        bool status;
        std::string_view::iterator n, m;
        std::string_view::iterator n_1;
        int64_t begin, length;
        while (true) {
            n = std::find(pos, pattern.end(), '{');
            if (n == pattern.end()) {
                begin = pos - pattern.begin();
                length = n - pos;
                builder.append(pattern.data() + begin, length);
                status = false;
                break;
            }
            if (n != pattern.begin() && *(n - 1) == '\\') {
                begin = pos - pattern.begin();
                length = n - pos;
                builder.append(pattern.data() + begin, length);
                builder << '{';
                pos = n + 1;
                continue;
            }
            n_1 = n;
        find_m:
            m = std::find(n_1, pattern.end(), '}');
            if (m == pattern.end()) {
                status = false;
                break;
            }
            if (m != pattern.begin() && *(m - 1) == '\\') {
                n_1 = m + 1;
                goto find_m;
            }
            args = std::string(n + 1, m);
            begin = pos - pattern.begin();
            length = n - pos;
            builder.append(pattern.data() + begin, length);
            pos = m + 1;
            status = true;
            break;
        }
        return status;
    }
};

constexpr size_t FormatParameterCounter(const char *pattern) {
    size_t count = 0;
    const char *p = pattern;
    if (*p == '{') count += 1;
    p++;
    while (*p != 0) {
        if (*p == '{' && *(p - 1) != '\\') {
            count += 1;
        }
        p++;
    }
    return count;
}

template<const char *PATTERN, typename... ARGS, typename std::enable_if<sizeof...(ARGS) != 0, int>::type = 0>
constexpr bool FormatCheck(ARGS &&...) {
    constexpr size_t PARAM_COUNT = FormatParameterCounter(PATTERN);
    constexpr size_t ARGS_COUNT = sizeof...(ARGS);
    return PARAM_COUNT == ARGS_COUNT;
}

template<typename T>
void Format(FmtCtx &ctx, T &&arg) {
    std::string parsing_args;
    auto status = ctx.constantParsing(parsing_args);
    if (status) {
        auto formatter = overload::Formatter<std::decay_t<T>>();
        if (!parsing_args.empty()) {
            formatter.parse(parsing_args);
        }
        ctx.builder << formatter.format(std::forward<T>(arg));
        [[maybe_unused]] auto result = ctx.constantParsing(parsing_args);
        assert(false == result);
    }
}

template<typename T, typename... ARGS>
void Format(FmtCtx &ctx, T &&arg, ARGS &&...args) {
    std::string parsing_args;
    auto status = ctx.constantParsing(parsing_args);
    if (status) {
        auto formatter = overload::Formatter<std::decay_t<T>>();
        if (!parsing_args.empty()) {
            formatter.parse(parsing_args);
        }
        ctx.builder << formatter.format(std::forward<T>(arg));
        Format(ctx, std::forward<ARGS>(args)...);
    }
}

/// 字符串格式化
/// \tparam ARGS 形参
/// \param pattern 匹配格式
/// \return 匹配完成的字符串
template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) == 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS &&...) {
    return {pattern.begin(), pattern.end()};
}

/// 字符串格式化
/// \tparam ARGS 形参
/// \param pattern 匹配格式
/// \param args 实参
/// \return 匹配完成的字符串
template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) != 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS &&...args) {
    FmtCtx ctx(pattern);
    Format(ctx, std::forward<ARGS>(args)...);
    return ctx.builder.toString();
}

} // namespace sese::text