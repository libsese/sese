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

// 占位符和实际参数个数不匹配 -> 模板参数展开
// 可自定义匹配类型 -> 模板偏特化
// 无参 -> 直接拷贝
// todo 简单的编译时计算
// todo 占位符参数

#pragma once

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>

#include <cassert>

namespace sese::text {

namespace overload {

    template<class T>
    std::string toString(T t) {
        return std::to_string(t);
    }

    template<>
    inline std::string toString(std::string t) {
        return t;
    }

    template<>
    inline std::string toString(const char *t) {
        return t;
    }
} // namespace overload

struct FmtCtx {
    std::string_view pattern;
    std::string_view::const_iterator pos;
    StringBuilder builder;
    uint16_t args;

    explicit FmtCtx(std::string_view p);

    bool constantParsing();
};

template<typename T>
void Format(FmtCtx &ctx, T arg) {
    auto status = ctx.constantParsing();
    if (status) {
        ctx.builder << overload::toString<T>(arg);
        assert(false == ctx.constantParsing());
    }
}

template<typename T, typename... ARGS>
void Format(FmtCtx &ctx, T arg, ARGS... args) {
    auto status = ctx.constantParsing();
    if (status) {
        ctx.builder << overload::toString<T>(arg);
        Format(ctx, args...);
    }
}

template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) == 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS... args) {
    return { pattern.begin(), pattern.end() };
}

template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) != 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS... args) {
    FmtCtx ctx(pattern);
    Format(ctx, args...);
    return ctx.builder.toString();
}

} // namespace sese::text