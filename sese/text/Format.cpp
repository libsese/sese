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

namespace sese::text {

FmtCtx::FmtCtx(std::string_view p) : pattern(p), pos(pattern.begin()) {
}

bool FmtCtx::constantParsing() {
    bool status;
    std::string_view::iterator n, m;
    std::string_view::iterator n_1;
    int64_t begin, length;
    while (true) {
        n = std::find(pos, pattern.end(), '{');
        if (n == pattern.end()) {
            begin = pos - pattern.begin();
            length = n - pos;
            builder.append(pattern.data() + begin, length);
            status = false;
            break;
        }
        if (n != pattern.begin() && *(n - 1) == '\\') {
            begin = pos - pattern.begin();
            length = n - pos;
            builder.append(pattern.data() + begin, length);
            builder << '{';
            pos = n + 1;
            continue;
        }
        n_1 = n;
    find_m:
        m = std::find(n_1, pattern.end(), '}');
        if (m == pattern.end()) {
            status = false;
            break;
        }
        if (m != pattern.begin() && *(m - 1) == '\\') {
            n_1 = m + 1;
            goto find_m;
        }
        begin = pos - pattern.begin();
        length = n - pos;
        builder.append(pattern.data() + begin, length);
        pos = m + 1;
        status = true;
        break;
    }
    return status;
}
} // namespace sese::text
