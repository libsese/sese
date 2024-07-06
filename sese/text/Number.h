/// \file Number.h
/// \brief 数字转字符串工具
/// \author kaoru
/// \date 2023年11月1日


#pragma once

#include <sese/util/NotInstantiable.h>
#include <sese/text/StringBuilder.h>

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

    template <typename T>
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
            int index = number  % radix;
            builder.append(DIGIT[index]);
            number /= radix;
            count += 1;
        } while (number > 0);

        // 直接反转 builder 中部分内存
        std::reverse(static_cast<char *>(builder.buf()) + builder.length() - count, static_cast<char *>(builder.buf()) + builder.length());
    }

private:
    static std::string toStringWithNoLeadingZeros(const std::string &number) noexcept;
};
} // namespace sese::text
