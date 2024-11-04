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

#include <sese/util/RandomUtil.h>

#include <sese/text/StringBuilder.h>

std::random_device sese::RandomUtil::dev{};

const std::string sese::RandomUtil::UPPER_LETTER_LIST = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string sese::RandomUtil::LOWER_LETTER_LIST = "abcdefghijklmnopqrstuvwxyz";
const std::string sese::RandomUtil::DIGIT_LIST = "01234567890";
const std::string sese::RandomUtil::SYMBOL_LIST = "@#$&*()'\"%-+=/;:!,?._^[]{}|~\\<>";

uint64_t sese::RandomUtil::nextUnsignedInt64() {
    return dev();
}

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
        size_t letter_size;
        if (less < size * 2) {
            letter_size = less;
        } else {
            letter_size = std::min(less, size);
        }
        less -= letter_size;
        for (int i = 0; i < letter_size; ++i) {
            seq.append(UPPER_LETTER_LIST.at(next<size_t>(0, UPPER_LETTER_LIST.length())));
        }
    }
    if (lower) {
        size_t letter_size;
        if (less < size * 2) {
            letter_size = less;
        } else {
            letter_size = std::min(less, size);
        }
        less -= letter_size;
        for (int i = 0; i < letter_size; ++i) {
            seq.append(LOWER_LETTER_LIST.at(next<size_t>(0, LOWER_LETTER_LIST.length())));
        }
    }
    if (symbol) {
        size_t symbol_size;
        if (less < size * 2) {
            symbol_size = less;
        } else {
            symbol_size = std::min(less, size);
        }
        less -= symbol_size;
        for (int i = 0; i < symbol_size; ++i) {
            seq.append(SYMBOL_LIST.at(next<size_t>(0, SYMBOL_LIST.length())));
        }
    }
    if (digit) {
        size_t digit_size;
        if (less < size * 2) {
            digit_size = less;
        } else {
            digit_size = std::min(less, size);
        }
        less -= digit_size;
        for (int i = 0; i < digit_size; ++i) {
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

std::random_device &sese::RandomUtil::getDevInstance() noexcept {
    return dev;
}
