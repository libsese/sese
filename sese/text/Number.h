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

/// \file Number.h
/// \brief 数字转字符串工具
/// \author kaoru
/// \date 2023年11月1日


#pragma once

#include <sese/util/NotInstantiable.h>
#include <sese/text/StringBuilder.h>
#include <sese/Util.h>
#include <cmath>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable: 4146)
#endif

namespace sese::text {
/// 数字转字符串工具
class Number final : public NotInstantiable {
public:
    Number() = delete;

    static std::string toHex(uint64_t number, bool upper_case = true) noexcept;

    static std::string toHex(int64_t number, bool upper_case = true) noexcept;

    static std::string toOct(uint64_t number) noexcept;

    static std::string toOct(int64_t number) noexcept;

    static std::string toBin(uint64_t number) noexcept;

    static std::string toBin(int64_t number) noexcept;

    static std::string toString(double number, uint16_t placeholder) noexcept;

    template<typename T>
    static void toString(StringBuilder &builder, T number, int radix, bool upper_case) noexcept {
        if (number < 0) {
            builder.append('-');
            number = -number;
        }
        int count = 0;
        constexpr auto DIGIT_UP = "0123456789ABCDEF";
        constexpr auto DIGIT_DOWN = "0123456789abcdef";
        const auto DIGIT = upper_case ? DIGIT_UP : DIGIT_DOWN;
        do {
            int index = number % radix;
            builder.append(DIGIT[index]);
            number /= radix;
            count += 1;
        } while (number > 0);

        // 直接反转 builder 中部分内存
        std::reverse(static_cast<char *>(builder.buf()) + builder.length() - count, static_cast<char *>(builder.buf()) + builder.length());
    }

    template<typename T>
    static void toString(StringBuilder &builder, T number, uint16_t placeholder) noexcept {
        auto int_part = static_cast<int64_t>(number);
        double frac = number - int_part;
        double rounding_factor = std::pow(10, placeholder);
        auto rounded_part = static_cast<int64_t>(frac * rounding_factor);

        if (static_cast<double>(rounded_part) >= rounding_factor) {
            int_part += 1;
            rounded_part -= static_cast<int64_t>(rounding_factor);
        }

        toString(builder, int_part, 10, true);
        if (placeholder > 0) {
            builder.append('.');
            if (rounded_part < 0) {
                rounded_part = -rounded_part;
            }
            toString(builder, rounded_part, 10, true);
            auto len = number2StringLength(rounded_part, 10);
            if (len < placeholder) {
                builder.append(std::string(placeholder - len, '0'));
            }
        }
    }

private:
    static std::string toStringWithNoLeadingZeros(const std::string &number) noexcept;
};
} // namespace sese::text
