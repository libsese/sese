/// \file Number.h
/// \brief 数字转字符串工具
/// \author kaoru
/// \date 2023年11月1日


#pragma once

#include <sese/util/NotInstantiable.h>

namespace sese::text {
/// 数字转字符串工具
class Number : public NotInstantiable {
public:
    static std::string toHex(uint64_t number, bool upperCase = true) noexcept;

    static std::string toHex(int64_t number, bool upperCase = true) noexcept;

    static std::string toOct(uint64_t number) noexcept;

    static std::string toOct(int64_t number) noexcept;

    static std::string toBin(uint64_t number) noexcept;

    static std::string toBin(int64_t number) noexcept;

private:
    static std::string toStringWithNoLeadingZeros(const std::string &number) noexcept;
};
} // namespace sese::text
