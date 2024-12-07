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

#include <benchmark/benchmark.h>
#include <sese/text/FormatOption.h>

using sese::text::Align;
using sese::text::FormatOption;

constexpr auto PATTERN1 = ": >12d";
constexpr auto PATTERN2 = ":<6.6f";
constexpr auto PATTERN3 = ":x";
constexpr auto PATTERN4 = ": ^5.5f";

class MockFormatOption1 : FormatOption {
public:
    bool parse(const std::string &value) {
        std::string::const_iterator pos = value.begin();
        if (*pos != ':') {
            return false;
        }
        if (pos + 1 == value.end()) {
            return true;
        }
        pos += 1;

    parsing_align:
        if (*pos == '<' || *pos == '>' || *pos == '^') {
            if (*pos == '<') {
                align = Align::LEFT;
            } else if (*pos == '>') {
                align = Align::RIGHT;
            } else {
                align = Align::CENTER;
            }
            if (pos + 1 == value.end()) {
                return true;
            }
            pos += 1;
        } else if (wide_char == ' ') {
            if (pos + 1 == value.end()) {
                ext_type = *pos;
                return true;
            }
            wide_char = *pos;
            pos += 1;
            goto parsing_align;
        } else {
            wide_char = ' ';
            pos -= 1;
        }

        char *end;
        wide = static_cast<uint16_t>(std::strtol(value.c_str() + (pos - value.begin()), &end, 10));
        if (*end == 0) {
            return true;
        }

        if (*end == '.') {
            char *new_end;
            float_placeholder = static_cast<uint16_t>(std::strtol(end + 1, &new_end, 10));
            if (end + 1 == new_end) {
                // Lack of accuracy
                return false;
            }
            end = new_end;
        }
        ext_type = *end;
        if (*(end + 1) == 0) {
            return false;
        }
        return true;
    }
};

class MockFormatOption2 : FormatOption {
public:
    SESE_ALWAYS_INLINE static bool is_align(char ch) {
        return ch == '<' || ch == '>' || ch == '^';
    }

    SESE_ALWAYS_INLINE static Align delect_align(char ch) { // NOLINT
        if (ch == '<') {
            return Align::LEFT;
        }
        if (ch == '>') {
            return Align::RIGHT;
        }
        if (ch == '^') {
            return Align::CENTER;
        }
        assert(false);
        return Align::LEFT;
    }

    bool parse(const std::string &value) {
        if (value[0] != ':') {
            return false;
        }

        // This can only be the extended type
        if (value.size() == 2 && !is_align(value[1])) {
            ext_type = value[1];
            return true;
        }

        // Align relevant judgments
        bool has_align = false;
        auto pos = value.begin() + 1;
        if (!is_align(*pos)) {
            if (pos + 1 != value.end() && is_align(*(pos + 1))) {
                wide_char = *pos;
                pos += 1;
                has_align = true;
            }
        } else {
            has_align = true;
        }
        if (has_align) {
            align = delect_align(*pos);
            pos += 1;
        }
        if (pos == value.end()) {
            return true;
        }
        char *end;
        wide = static_cast<decltype(wide)>(std::strtol(value.data() + (pos -value.begin()), &end, 10));
        pos = value.begin() + (end - value.data());

        // If end is not \0, it will be returned directly, without additional judgment
        // Judgment related to floating-point precision
        if (*end == '.') {
            char *new_end;
            float_placeholder = static_cast<decltype(float_placeholder)>(std::strtol(end + 1, &new_end, 10));
            if (end == new_end) {
                // Lack of precision
                return false;
            }
            end = new_end;
            pos = value.begin() + (end - value.data());
        }
        if (pos == value.end()) {
            return true;
        }

        // Expand type judgment
        ext_type = *pos;
        if (pos + 1 == value.end()) {
            return true;
        }

        return false;
    }
};

static void BM_Parse0(benchmark::State &state) {
    for (auto _: state) {
        FormatOption option;
        option.parse(PATTERN1);
        option.parse(PATTERN2);
        option.parse(PATTERN3);
        option.parse(PATTERN4);
    }
}

static void BM_Parse1(benchmark::State &state) {
    for (auto _: state) {
        MockFormatOption1 option;
        option.parse(PATTERN1);
        option.parse(PATTERN2);
        option.parse(PATTERN3);
        option.parse(PATTERN4);
    }
}

static void BM_Parse2(benchmark::State &state) {
    for (auto _: state) {
        MockFormatOption2 option;
        option.parse(PATTERN1);
        option.parse(PATTERN2);
        option.parse(PATTERN3);
        option.parse(PATTERN4);
    }
}

BENCHMARK(BM_Parse0);
BENCHMARK(BM_Parse1);
BENCHMARK(BM_Parse2);
BENCHMARK_MAIN();
