#include <sese/text/Number.h>

#include <bitset>

using sese::text::Number;

std::string Number::toHex(uint64_t number, bool upper_case) noexcept {
    StringBuilder builder;
    toString(builder, number, 16, upper_case);
    return builder.toString();
}

std::string Number::toHex(int64_t number, bool upper_case) noexcept {
    StringBuilder builder(32);
    toString(builder, number, 16, upper_case);
    return builder.toString();
}

std::string Number::toOct(uint64_t number) noexcept {
    StringBuilder builder(32);
    toString(builder, number, 8, true);
    return builder.toString();
}

std::string Number::toOct(int64_t number) noexcept {
    StringBuilder builder(32);
    toString(builder, number, 8, true);
    return builder.toString();
}

std::string Number::toBin(uint64_t number) noexcept {
    std::bitset<64> bits(number);
    return toStringWithNoLeadingZeros(bits.to_string());
}

std::string Number::toBin(int64_t number) noexcept {
    std::bitset<64> bits(number);
    return toStringWithNoLeadingZeros(bits.to_string());
}

std::string Number::toStringWithNoLeadingZeros(const std::string &number) noexcept {
    size_t found = number.find_first_not_of('0');
    if(found != std::string::npos) {
        return number.substr(found);
    }
    return "0";
}