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
    text::StringBuilder args_builder(UINT8_MAX);
    std::string_view::iterator n, m;
    std::string_view::iterator pre_n, pre_m;
    while (true) {
        // '{' 查找并提取常量
        pre_n = pos;
    find_n:
        n = std::find(pre_n, pattern.end(), '{');
        if (n == pattern.end()) {
            // 直接写入常量
            std::string substr(pre_n, n);
            builder.append(substr);
            status = false;
            pos = n;
            break;
        }
        if (n != pattern.begin() && *(n - 1) == '\\') {
            // 找到但是是转义字符
            std::string substr(pre_n, n - 1);
            builder.append(substr);
            builder << '{';
            pre_n = n + 1;
            goto find_n;
        }
        {
            // 有效 '{'
            std::string substr(pre_n, n);
            builder.append(substr);
            pre_m = n + 1;
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
                std::string substr(pre_m, m - 1);
                args_builder << substr;
                args_builder << '}';
                pre_m = m + 1;
                goto find_m;
            } else {
                // 有效 '}'
                std::string substr(pre_m, m);
                args_builder << substr;
                args = args_builder.toString();
                status = true;
                pos = m + 1;
                break;
            }
        }
    }
    return status;
}

bool sese::text::FormatOption_StringParse(sese::text::FormatOption &opt, const std::string &opt_str) {
    auto status = opt.parse(opt_str);
    if (!status) {
        return false;
    }
    if (opt.ext_type != 0) {
        return false;
    }
    if (opt.float_accuracy != 0) {
        return false;
    }
    return true;
}

std::string sese::text::FormatOption_StringFormat(sese::text::FormatOption &opt, const std::string &value) {
    if (opt.wide <= value.length()) {
        return value;
    }
    auto diff = opt.wide - value.length();
    switch (opt.align) {
        case Align::LEFT:
            return value + std::string(diff, opt.wide_char);
        case Align::RIGHT:
            return std::string(diff, opt.wide_char) + value;
        case Align::CENTER:
            return std::string(diff / 2, opt.wide_char) +
                   value +
                   std::string((diff % 2 == 1 ? (diff / 2 + 1) : (diff / 2)), opt.wide_char);
    }
    return {};
}
