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

#include <sese/text/Format.h>

sese::text::FmtCtx::FmtCtx(std::string_view p) : pattern(p), pos(pattern.begin()) {}

SESE_ALWAYS_INLINE bool isKeyWord(char c) {
    return c == '{' || c == '}';
}

bool sese::text::FmtCtx::parsing(std::string &args) {
    // { asdasdasd }
    // ^           ^^
    // pos1    pos2  pos
    bool in_args = false;
    std::unique_ptr<StringBuilder> args_builder;
    std::string_view::const_iterator pos1 = pos, pos2;
    while (pos1 != pattern.end()) {
        pos2 = std::find_if(pos1, pattern.end(), isKeyWord);
        if (in_args) {
            // 缺少闭包 '}'
            if (pos2 == pattern.end()) {
                return false;
            }
            // 转义字符
            if (pos2 != pattern.begin() && *(pos2 - 1) == '\\') {
                if (args_builder == nullptr) {
                    args_builder = std::make_unique<StringBuilder>();
                }
                args_builder->append(pattern.data() + (pos1 - pattern.begin()), pos2 - pos1 - 1);
                args_builder->append(*pos2);
                pos1 = pos2 + 1;
                continue;
            }
            // 闭包字符
            if (*pos2 == '}') {
                in_args = false;
                if (args_builder) {
                    args = args_builder->toString();
                } else {
                    args = {pos1, pos2};
                }
                pos = pos2 + 1;
                break;
            }
        } else {
            // 没有开包
            if (pos2 == pattern.end()) {
                builder.append(pattern.data() + (pos1 - pattern.begin()), pos2 - pos1);
                pos = pattern.end();
                return true;
            }
            // 转义字符
            if (pos2 != pattern.begin() && *(pos2 - 1) == '\\') {
                builder.append(pattern.data() + (pos1 - pattern.begin()), pos2 - pos1 - 1);
                builder.append(*pos2);
                pos1 = pos2 + 1;
                continue;
            }
            // 开包字符
            if (*pos2 == '{') {
                builder.append(pattern.data() + (pos1 - pattern.begin()), pos2 - pos1);
                pos1 = pos2 + 1;
                in_args = true;
            }
        }
    }
    assert(!in_args);
    return true;
}

bool sese::text::FormatOption_StringParse(FormatOption &opt, const std::string &opt_str) {
    auto status = opt.parse(opt_str);
    if (!status) {
        return false;
    }
    if (opt.ext_type != 0) {
        return false;
    }
    if (opt.float_placeholder != 0) {
        return false;
    }
    return true;
}

void sese::text::FormatOption_StringFormat(FmtCtx &ctx, FormatOption &opt, const std::string &value) {
    StringBuilder &builder = ctx.builder;
    if (opt.wide <= value.length()) {
        builder << value;
        return;
    }
    auto diff = opt.wide - value.length();
    switch (opt.align) {
        case Align::LEFT:
            builder << value + std::string(diff, opt.wide_char);
            break;
        case Align::RIGHT:
            builder << std::string(diff, opt.wide_char) + value;
            break;
        case Align::CENTER:
            builder << std::string(diff / 2, opt.wide_char) +
                               value +
                               std::string((diff % 2 == 1 ? (diff / 2 + 1) : (diff / 2)), opt.wide_char);
            break;
    }
}

bool sese::text::FormatOption_NumberParse(FormatOption &opt, const std::string &opt_str) {
    auto status = opt.parse(opt_str);
    if (!status) {
        return false;
    }

    switch (opt.ext_type) {
        case 'f':
        case '%':
            break;
        case 'X':
        case 'x':
        case 'o':
        case 'b':
        case 'd':
        case '\0':
            if (opt.float_placeholder != 0) {
                return false;
            }
            break;
        default:
            return false;
    }
    return true;
}
