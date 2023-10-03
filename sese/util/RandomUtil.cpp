#include <sese/util/RandomUtil.h>

#include <sese/text/StringBuilder.h>

sese::Random sese::RandomUtil::dev{};

std::string sese::RandomUtil::UPPER_LETTER_LIST = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::string sese::RandomUtil::LOWER_LETTER_LIST = "abcdefghijklmnopqrstuvwxyz";
std::string sese::RandomUtil::DIGIT_LIST = "01234567890";
std::string sese::RandomUtil::SYMBOL_LIST = "@#$&*()'\"%-+=/;:!,?._^[]{}|~\\<>";

std::string sese::RandomUtil::nextString(size_t length, int required) {
    bool upper = required & REQUIRED_UPPER_LETTER;
    bool lower = required & REQUIRED_LOWER_LETTER;
    bool digit = required & REQUIRED_DIGIT;
    bool symbol = required & REQUIRED_SYMBOL;
    if (upper || lower || digit || symbol) {
        return sese::RandomUtil::nextString(length, upper, lower, digit, symbol);
    } else {
        return {};
    }
}

std::string sese::RandomUtil::nextString(size_t length, bool upper, bool lower, bool digit, bool symbol) {
    size_t types = 0;
    if (upper) types += 1;
    if (lower) types += 1;
    if (digit) types += 1;
    if (symbol) types += 1;

    sese::text::StringBuilder seq(length);
    size_t size = length / types;
    size_t less = length;
    if (upper) {
        size_t letterSize;
        if (less < size * 2) {
            letterSize = less;
        } else {
            letterSize = std::min(less, size);
        }
        less -= letterSize;
        for (int i = 0; i < letterSize; ++i) {
            seq.append(UPPER_LETTER_LIST.at(next<size_t>(0, UPPER_LETTER_LIST.length())));
        }
    }
    if (lower) {
        size_t letterSize;
        if (less < size * 2) {
            letterSize = less;
        } else {
            letterSize = std::min(less, size);
        }
        less -= letterSize;
        for (int i = 0; i < letterSize; ++i) {
            seq.append(LOWER_LETTER_LIST.at(next<size_t>(0, LOWER_LETTER_LIST.length())));
        }
    }
    if (symbol) {
        size_t symbolSize;
        if (less < size * 2) {
            symbolSize = less;
        } else {
            symbolSize = std::min(less, size);
        }
        less -= symbolSize;
        for (int i = 0; i < symbolSize; ++i) {
            seq.append(SYMBOL_LIST.at(next<size_t>(0, SYMBOL_LIST.length())));
        }
    }
    if (digit) {
        size_t digitSize;
        if (less < size * 2) {
            digitSize = less;
        } else {
            digitSize = std::min(less, size);
        }
        less -= digitSize;
        for (int i = 0; i < digitSize; ++i) {
            seq.append(DIGIT_LIST.at(next<size_t>(0, DIGIT_LIST.length())));
        }
    }

    auto res = seq.toString();

    auto time = length / 2;
    for (int i = 0; i < time; ++i) {
        auto a = next<size_t>(0, length);
        auto b = next<size_t>(0, length);
        std::swap(res[a], res[b]);
    }

    return res;
}
