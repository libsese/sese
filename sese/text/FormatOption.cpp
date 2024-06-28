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

#include <sese/text/FormatOption.h>

bool sese::text::FormatOption::parse(const std::string &value) {
    std::string::const_iterator pos = value.begin();
    if (*pos != ':') {
        return false;
    }
    if (pos + 1 == value.end()) {
        return true;
    }
    pos += 1;

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
    }

    char *end;
    wide = static_cast<uint16_t>(std::strtol(value.c_str() + (pos - value.begin()), &end, 10));
    if (*end == 0) {
        return true;
    }

    if (*end == '.') {
        char *new_end;
        float_accuracy = static_cast<uint16_t>(std::strtol(end + 1, &new_end, 10));
        if (end + 1 == new_end) {
            // 精度缺失
            return false;
        }
        end = new_end;
    }
    ext_type = *end;
    return true;
}
