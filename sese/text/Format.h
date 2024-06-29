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

/// @file Format.h
/// @brief 字符串格式化
/// @author kaoru
/// @date 2024年06月29日


#pragma once

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>
#include <sese/text/Number.h>
#include <sese/text/Util.h>
#include <sese/text/FormatOption.h>
#include <sese/types/is_iterable.h>

#include <cassert>
#include <cmath>

namespace sese::text {

/// 解析并校验字符串格式化选项
/// \param opt 选项
/// \param opt_str 选项字符串
/// \return 解析是否成功
bool FormatOption_StringParse(FormatOption &opt, const std::string &opt_str);

/// 按照格式化选项格式化字符串（对齐）
/// \param opt 选项
/// \param value 字符串
/// \return 格式化字符串
std::string FormatOption_StringFormat(FormatOption &opt, const std::string &value);

/// 解析并校验字符串格式化选项
/// \param opt 选项
/// \param opt_str 选项字符串
/// \return 解析是否成功
bool FormatOption_NumberParse(FormatOption &opt, const std::string &opt_str);

/// 按照整形格式化标准格式化字符串
/// \tparam T 整形类型
/// \param opt 选项
/// \param number 整形
/// \return 格式化字符串
template<typename T>
std::string FormatOption_NumberFormat(FormatOption &opt, T number) {
    auto radix = opt.ext_type;
    if (radix == 'X') {
        return Number::toHex(static_cast<T>(number), true);
    } else if (radix == 'x') {
        return Number::toHex(static_cast<T>(number), false);
    } else if (radix == 'o') {
        return Number::toOct(static_cast<T>(number));
    } else if (radix == 'b') {
        return Number::toBin(static_cast<T>(number));
    } else {
        return std::to_string(number);
    }
}

/// 按照浮点格式化标准格式化字符串
/// \tparam T 浮点类型
/// \param opt 选项
/// \param number 浮点数
/// \return 格式化字符串
template<typename T>
std::string FormatOption_FloatNumberFormat(FormatOption &opt, T number) {
    if (opt.float_placeholder == 0) {
        opt.float_placeholder = 1;
    }
    char buffer[256]{};
    if (opt.ext_type == '%') {
        number *= 100;
        const std::string PATTERN = "%." + std::to_string(opt.float_placeholder) + "f%%";
        sese::text::snprintf(buffer, sizeof(buffer), PATTERN.c_str(), number);
    } else {
        const std::string PATTERN = "%." + std::to_string(opt.float_placeholder) + "f";
        sese::text::snprintf(buffer, sizeof(buffer), PATTERN.c_str(), number);
    }
    return buffer;
}

namespace overload {

    template<typename VALUE, typename ENABLE = void>
    struct Formatter {
        bool parse(const std::string &) {
            return false;
        }

        std::string format(const VALUE &) {
            return {};
        }
    };

    template<>
    struct Formatter<std::string> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_StringParse(option, opt_str);
        }

        std::string format(const std::string &value) {
            return FormatOption_StringFormat(option, value);
        }
    };

    template<>
    struct Formatter<const char *> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_StringParse(option, opt_str);
        }
        std::string format(const char *value) {
            return FormatOption_StringFormat(option, value);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<std::is_integral_v<VALUE> && std::is_signed_v<VALUE>>> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_NumberParse(option, opt_str);
        }

        std::string format(const VALUE &value) {
            auto number = FormatOption_NumberFormat<int64_t>(option, value);
            return FormatOption_StringFormat(option, number);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<std::is_integral_v<VALUE> && std::is_unsigned_v<VALUE>>> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_NumberParse(option, opt_str);
        }

        std::string format(const VALUE &value) {
            auto number = FormatOption_NumberFormat<uint64_t>(option, value);
            return FormatOption_StringFormat(option, number);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<std::is_floating_point_v<VALUE>>> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_NumberParse(option, opt_str);
        }
        std::string format(const VALUE &value) {
            if(std::isnan(value)) {
                return "NaN";
            }
            auto number = FormatOption_FloatNumberFormat<VALUE>(option, value);
            return FormatOption_StringFormat(option, number);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<is_iterable_v<VALUE>>> {
        char begin_ch = '[';
        char end_ch = ']';
        char split_ch = ',';

        bool parse(const std::string &args) {
            if (args.size() == 3) {
                begin_ch = args[0];
                split_ch = args[1];
                end_ch = args[2];
                return true;
            }
            return false;
        }

        std::string format(VALUE &value) {
            StringBuilder builder;
            builder << begin_ch;
            bool first = true;
            for (auto &&item: value) {
                auto formatter = overload::Formatter<std::decay_t<typename VALUE::value_type>>();
                if (first) {
                    first = false;
                } else {
                    builder << split_ch;
                }
                builder << formatter.format(item);
            }
            builder << end_ch;
            return builder.toString();
        }
    };

} // namespace overload

struct FmtCtx {
    StringBuilder builder;
    std::string_view pattern;
    std::string_view::const_iterator pos;

    explicit FmtCtx(std::string_view p);

    bool parsing(std::string &args);
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

template<typename T>
void Format(FmtCtx &ctx, T &&arg) {
    std::string parsing_args;
    auto status = ctx.parsing(parsing_args);
    if (!status) {
        return;
    }
    auto formatter = overload::Formatter<std::decay_t<T>>();
    if (!parsing_args.empty()) {
        if (formatter.parse(parsing_args)) {
            ctx.builder << formatter.format(std::forward<T>(arg));
        } else {
            ctx.builder << "!{parsing failed}";
        }
        ctx.parsing(parsing_args);
    } else {
        ctx.builder << formatter.format(std::forward<T>(arg));
        ctx.parsing(parsing_args);
    }
}

template<typename T, typename... ARGS>
void Format(FmtCtx &ctx, T &&arg, ARGS &&...args) {
    std::string parsing_args;
    auto status = ctx.parsing(parsing_args);
    if (!status) {
        return;
    }
    auto formatter = overload::Formatter<std::decay_t<T>>();
    if (!parsing_args.empty()) {
        if (formatter.parse(parsing_args)) {
            ctx.builder << formatter.format(std::forward<T>(arg));
        } else {
            ctx.builder << "!{parsing failed}";
        }
        Format(ctx, std::forward<ARGS>(args)...);
    } else {
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
    auto param = FormatParameterCounter(pattern.data());
    if (param) {
        return "!{Mismatch in number of parameters}";
    }
    return {pattern.begin(), pattern.end()};
}

/// 字符串格式化
/// \tparam ARGS 形参
/// \param pattern 匹配格式
/// \param args 实参
/// \return 匹配完成的字符串
template<typename... ARGS, typename std::enable_if<sizeof...(ARGS) != 0, int>::type = 0>
std::string fmt(std::string_view pattern, ARGS &&...args) {
    auto param = FormatParameterCounter(pattern.data());
    if (param != sizeof...(args)) {
        return "!{Mismatch in number of parameters}";
    }
    FmtCtx ctx(pattern);
    Format(ctx, std::forward<ARGS>(args)...);
    return ctx.builder.toString();
}

} // namespace sese::text