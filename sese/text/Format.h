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
/// @brief String formatting
/// @author kaoru
/// @date June 29, 2024

#pragma once

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>
#include <sese/text/Number.h>
#include <sese/text/FormatOption.h>
#include <sese/types/is_iterable.h>
#include <sese/types/is_pair.h>
#include <sese/Util.h>

#include <cassert>
#include <cmath>

namespace sese::text {

struct FmtCtx {
    StringBuilder builder;
    std::string_view pattern;
    std::string_view::const_iterator pos;

    explicit FmtCtx(std::string_view p);

    bool parsing(std::string &args);
};

/// Parse and validate string formatting options
/// \param opt Option
/// \param opt_str Option string
/// \return Whether the parsing was successful
bool FormatOption_StringParse(FormatOption &opt, const std::string &opt_str);

/// Format string according to formatting options (alignment)
/// \param ctx Context
/// \param opt Options
/// \param value String
void FormatOption_StringFormat(FmtCtx &ctx, FormatOption &opt, const std::string &value);

/// Parse and validate string formatting options
/// \param opt Option
/// \param opt_str Option string
/// \return Whether the parsing was successful
bool FormatOption_NumberParse(FormatOption &opt, const std::string &opt_str);

template<typename T>
SESE_ALWAYS_INLINE void FormatOption_NumberFormatAlgin(FmtCtx &ctx, FormatOption &opt, T number, int radix, bool upper_case) {
    StringBuilder &builder = ctx.builder;
    auto len = number2StringLength(number, radix);
    if (opt.wide <= len) {
        Number::toString(builder, number, radix, upper_case);
        return;
    }
    auto diff = opt.wide - len;
    switch (opt.align) {
        case Align::LEFT:
            Number::toString(builder, number, radix, upper_case);
            builder << std::string(diff, opt.wide_char); // GCOVR_EXCL_LINE
            break;
        case Align::RIGHT:
            builder << std::string(diff, opt.wide_char); // GCOVR_EXCL_LINE
            Number::toString(builder, number, radix, upper_case);
            break;
        case Align::CENTER:
            builder << std::string(diff / 2, opt.wide_char); // GCOVR_EXCL_LINE
            Number::toString(builder, number, radix, upper_case);
            builder << std::string((diff % 2 == 1 ? (diff / 2 + 1) : (diff / 2)), opt.wide_char); // GCOVR_EXCL_LINE
            break;
    }
}

/// Format string according to integer formatting standards, including alignment
/// \tparam T Integer type
/// \param ctx Formatting context
/// \param opt Options
/// \param number Integer
template<typename T>
void FormatOption_NumberFormat(FmtCtx &ctx, FormatOption &opt, T number) {
    auto radix = opt.ext_type;
    if (radix == 'X') {
        FormatOption_NumberFormatAlgin(ctx, opt, number, 16, true);
    } else if (radix == 'x') {
        FormatOption_NumberFormatAlgin(ctx, opt, number, 16, false);
    } else if (radix == 'o') {
        FormatOption_NumberFormatAlgin(ctx, opt, number, 8, true);
    } else if (radix == 'b') {
        FormatOption_NumberFormatAlgin(ctx, opt, number, 2, true);
    } else {
        FormatOption_NumberFormatAlgin(ctx, opt, number, 10, true);
    }
}

/// Format string according to floating-point formatting standards
/// \tparam T Floating-point type
/// \param ctx Formatting context
/// \param opt Options
/// \param number Floating-point number
template<typename T>
void FormatOption_FloatNumberFormat(FmtCtx &ctx, FormatOption &opt, T number) {
    if (opt.float_placeholder == 0) {
        opt.float_placeholder = 1;
    }
    StringBuilder &builder = ctx.builder;
    size_t len;
    if (opt.ext_type == '%') {
        number *= 100;
        len = floating2StringLength(number, opt.float_placeholder);
        len += 1;
    } else {
        len = floating2StringLength(number, opt.float_placeholder);
    }
    if (opt.wide <= len) {
        Number::toString(builder, number, opt.float_placeholder);
        if (opt.ext_type == '%') {
            builder.append('%');
        }
        return;
    }
    auto diff = opt.wide - len;
    switch (opt.align) {
        case Align::LEFT:
            Number::toString(builder, number, opt.float_placeholder);
            if (opt.ext_type == '%') {
                builder.append('%');
            }
            builder << std::string(diff, opt.wide_char); // GCOVR_EXCL_LINE
            break;
        case Align::RIGHT:
            builder << std::string(diff, opt.wide_char); // GCOVR_EXCL_LINE
            Number::toString(builder, number, opt.float_placeholder);
            if (opt.ext_type == '%') {
                builder.append('%');
            }
            break;
        case Align::CENTER:
            builder << std::string(diff / 2, opt.wide_char); // GCOVR_EXCL_LINE
            Number::toString(builder, number, opt.float_placeholder);
            if (opt.ext_type == '%') {
                builder.append('%');
            }
            builder << std::string((diff % 2 == 1 ? (diff / 2 + 1) : (diff / 2)), opt.wide_char); // GCOVR_EXCL_LINE
            break;
    }
}

namespace overload {
    template<typename VALUE, typename ENABLE = void>
    struct Formatter {
        bool parse(const std::string &) { return false; }

        void format(FmtCtx &, const VALUE &) {}
    };

    template<>
    struct Formatter<std::string> {
        FormatOption option;

        bool parse(const std::string &opt_str) { return FormatOption_StringParse(option, opt_str); }

        void format(FmtCtx &ctx, const std::string &value) { FormatOption_StringFormat(ctx, option, value); }
    };

    template<>
    struct Formatter<const char *> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_StringParse(option, opt_str);
        }

        void format(FmtCtx &ctx, const char *value) {
            FormatOption_StringFormat(ctx, option, value);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<std::is_integral_v<VALUE>>> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_NumberParse(option, opt_str);
        }

        void format(FmtCtx &ctx, const VALUE &value) {
            FormatOption_NumberFormat(ctx, option, value);
        }
    };

    template<typename VALUE>
    struct Formatter<VALUE, std::enable_if_t<std::is_floating_point_v<VALUE>>> {
        FormatOption option;

        bool parse(const std::string &opt_str) {
            return FormatOption_NumberParse(option, opt_str);
        }

        void format(FmtCtx &ctx, const VALUE &value) {
            if (std::isnan(value)) {
                ctx.builder << "NaN";
            } else {
                FormatOption_FloatNumberFormat<VALUE>(ctx, option, value);
            }
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

        void format(FmtCtx &ctx, VALUE &value) {
            StringBuilder &builder = ctx.builder;
            builder << begin_ch;
            bool first = true;
            for (auto &&item: value) {
                auto formatter = overload::Formatter<std::decay_t<typename VALUE::value_type>>();
                if (first) {
                    first = false;
                } else {
                    builder << split_ch;
                }
                formatter.format(ctx, item);
            }
            builder << end_ch;
        }
    };

} // namespace overload

template<typename C, std::enable_if_t<!is_pair<typename C::value_type>::value, int> = 0>
std::string for_each(const C &container) {
    constexpr char SPLIT_CH = ',';
    constexpr char BEGIN_CH = '[';
    constexpr char END_CH = ']';
    if (container.empty()) {
        return "[]";
    }
    FmtCtx ctx("");
    auto formatter = overload::Formatter<std::decay_t<typename C::value_type>>();
    bool first = true;
    ctx.builder << BEGIN_CH;
    for (const auto &item: container) {
        if (first) {
            first = false;
        } else {
            ctx.builder << SPLIT_CH << ' ';
        }
        formatter.format(ctx, item);
    }
    ctx.builder << END_CH;
    return ctx.builder.toString();
}

template<typename C, std::enable_if_t<is_pair<typename C::value_type>::value, int> = 0>
std::string for_each(const C &container) {
    constexpr char SPLIT_CH = ',';
    constexpr char BEGIN_CH = '[';
    constexpr char END_CH = ']';
    constexpr char PAIR_BEGIN_CH = '{';
    constexpr char PAIR_END_CH = '}';
    if (container.empty()) {
        return "[]";
    }
    FmtCtx ctx("");
    auto key_formatter = overload::Formatter<std::decay_t<typename C::key_type>>();
    auto value_formatter = overload::Formatter<std::decay_t<typename C::mapped_type>>();
    bool first = true;
    ctx.builder << BEGIN_CH;
    for (auto &&[key, value]: container) {
        if (first) {
            first = false;
        } else {
            ctx.builder << SPLIT_CH << ' ';
        }
        ctx.builder << PAIR_BEGIN_CH;
        key_formatter.format(ctx, key);
        ctx.builder << SPLIT_CH << ' ';
        value_formatter.format(ctx, value);
        ctx.builder << PAIR_END_CH;
    }
    ctx.builder << END_CH;
    return ctx.builder.toString();
}

constexpr size_t FormatParameterCounter(const char *pattern) {
    size_t count = 0;
    const char *p = pattern;
    if (*p == '{')
        count += 1;
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
            formatter.format(ctx, std::forward<T>(arg));
        } else {
            ctx.builder << "!{parsing failed}";
        }
        ctx.parsing(parsing_args);
    } else {
        formatter.format(ctx, std::forward<T>(arg));
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
            formatter.format(ctx, std::forward<T>(arg));
        } else {
            ctx.builder << "!{parsing failed}";
        }
        Format(ctx, std::forward<ARGS>(args)...);
    } else {
        formatter.format(ctx, std::forward<T>(arg));
        Format(ctx, std::forward<ARGS>(args)...);
    }
}

/// String formatting
/// \tparam ARGS Template parameters
/// \param pattern Match pattern
/// \return Formatted string
template<typename... ARGS, std::enable_if_t<sizeof...(ARGS) == 0, int> = 0>
std::string fmt(std::string_view pattern, ARGS &&...) {
    if (FormatParameterCounter(pattern.data())) {
        return "!{Mismatch in number of parameters}";
    }
    return {pattern.begin(), pattern.end()};
}

/// String formatting
/// \tparam ARGS Template parameters
/// \param pattern Match pattern
/// \param args Arguments
/// \return Formatted string
template<typename... ARGS, std::enable_if_t<sizeof...(ARGS) != 0, int> = 0>
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