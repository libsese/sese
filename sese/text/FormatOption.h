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

#pragma once

#include <sese/Config.h>

namespace sese::text {

enum class Align {
    LEFT,
    CENTER,
    RIGHT
};

struct FormatOption {
    Align align = Align::LEFT;
    char wide_char = ' ';
    uint16_t wide = 0;
    uint16_t float_placeholder = 0;
    char ext_type = 0;

    SESE_ALWAYS_INLINE static bool is_align(const char CH) {
        return CH == '<' || CH == '>' || CH == '^';
    }

    SESE_ALWAYS_INLINE static Align delect_align(const char CH) {
        if (CH == '<') {
            return Align::LEFT;
        }
        if (CH == '>') {
            return Align::RIGHT;
        }
        if (CH == '^') {
            return Align::CENTER;
        }
        assert(false);
        return Align::LEFT;
    }

    bool parse(const std::string &value) {
        if (value[0] != ':') {
            return false;
        }

        // 这种情况只可能是拓展类型
        if (value.size() == 2 && !is_align(value[1])) {
            ext_type = value[1];
            return true;
        }

        // 对齐相关判断
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
        wide = static_cast<uint16_t>(std::strtol(value.data() + (pos - value.begin()), &end, 10));
        pos = value.begin() + (end - value.data());

        // end 不为 \0 将直接返回，无需额外判断
        // 浮点精度相关判断
        if (*end == '.') {
            char *new_end;
            float_placeholder = static_cast<uint16_t>(std::strtol(end + 1, &new_end, 10));
            if (end == new_end) {
                // 缺少精度
                return false;
            }
            end = new_end;
            pos = value.begin() + (end - value.data());
        }
        if (pos == value.end()) {
            return true;
        }

        // 拓展类型判断
        ext_type = *pos;
        if (pos + 1 == value.end()) {
            return true;
        }

        return false;
    }
};

} // namespace sese::text