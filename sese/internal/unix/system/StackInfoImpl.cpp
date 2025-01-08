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

#include <sese/system/StackInfo.h>
#include <sese/io/OutputBufferWrapper.h>

std::string sese::system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    char buffer[2000]{};
    sese::io::OutputBufferWrapper output(buffer, sizeof(buffer));

    if (!std::equal(str.begin(), str.begin() + 2, "_Z")) {
        return str;
    }

    int pos = str.c_str()[2] == 'N' ? 3 : 2;

    // _Z9function1v -> function1
    // _ZN1N1C4funcE -> N::C::func
    // _ZN18NamespaceStackInfo6ClassAC1Ev -> NamespaceStackInfo::ClassA::ClassA
    // _ZN18NamespaceStackInfo6ClassAD1Ev -> NamespaceStackInfo::ClassA::~ClassA
    std::string preword;
    bool first = true;
    char *p_str = const_cast<char *>(str.data()) + pos;
    char *p_next = nullptr;
    while (true) {
        auto len = std::strtoll(p_str, &p_next, 10);
        if (len == 0) {
            if (p_next[0] == 'D') {
                preword = "::~" + preword;
                output.write(preword.data(), preword.length());
                break;
            } else if (p_next[0] == 'C') {
                preword = "::" + preword;
                output.write(preword.data(), preword.length());
                break;
            } else {
                break;
            }
        }
        if (first) {
            first = false;
        } else {
            output.write("::", 2);
        }
        preword = std::string(p_next, len);
        output.write(p_next, len);
        p_str = p_next + len;
    }

    return buffer;
}