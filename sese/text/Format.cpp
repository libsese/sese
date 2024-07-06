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

bool sese::text::FmtCtx::parsing(std::string &args) {
    bool status;
    std::unique_ptr<StringBuilder> args_builder;
    std::string_view::iterator n, m;
    std::string_view::iterator pre_n, pre_m;
    while (true) {
        // '{' 查找并提取常量
        pre_n = pos;
    find_n:
        n = std::find(pre_n, pattern.end(), '{');
        if (n == pattern.end()) {
            // 直接写入常量
            // std::string substr(pre_n, n);
            auto begin = pattern.data() + (pre_n - pattern.begin());
            auto len = n - pre_n;
            builder.append(begin, len);
            status = false;
            pos = n;
            break;
        }
        if (n != pattern.begin() && *(n - 1) == '\\') {
            // 找到但是是转义字符
            // std::string substr(pre_n, n - 1);
            auto begin = pattern.data() + (pre_n - pattern.begin());
            auto len = n - pre_n - 1;
            builder.append(begin, len);
            builder << '{';
            pre_n = n + 1;
            goto find_n;
        }
        {
            // 有效 '{'
            // std::string substr(pre_n, n);
            auto begin = pattern.data() + (pre_n - pattern.begin());
            auto len = n - pre_n;
            builder.append(begin, len);
            pre_m = n + 1;
        }

        // 无参数直接返回
        if (*pre_m == '}') {
            return true;
        }

    find_m:
        m = std::find(pre_m, pattern.end(), '}');
        if (m == pattern.end()) {
            // 没有闭包，直接退出
            status = false;
            break;
        }
        if (m != pattern.begin()) {
            if (*(m - 1) == '\\') {
                // 找到但是是转义字符
                // std::string substr(pre_m, m - 1);
                auto begin = pattern.data() + (pre_m - pattern.begin());
                auto len = m - pre_m - 1;
                if (args_builder == nullptr) {
                    args_builder = std::make_unique<StringBuilder>(UINT8_MAX);
                }
                if (len) {
                    args_builder->append(begin, len);
                }
                args_builder->append('}');
                pre_m = m + 1;
                goto find_m;
            }
            // 有效 '}'
            // std::string substr(pre_m, m);
            auto begin = pattern.data() + (pre_m - pattern.begin());
            auto len = m - pre_m;
            if (len) {
                if (args_builder == nullptr) {
                    args = std::string(begin, len);
                } else {
                    args_builder->append(begin, len);
                    args = args_builder->toString();
                }
            }
            status = true;
            pos = m + 1;
            break;
        }
    }
    return status;
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
