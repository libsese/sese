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

#include "sese/net/http/Range.h"
#include "sese/text/StringBuilder.h"
#include "sese/Util.h"

sese::net::http::Range::Range(size_t begin, size_t len) noexcept {
    this->begin = begin;
    this->len = len;
}

std::vector<sese::net::http::Range> sese::net::http::Range::parse(const std::string &str, size_t total) noexcept {
    std::vector<sese::net::http::Range> vector;
    auto tmp0 = sese::text::StringBuilder::split(str, "=");
    if (tmp0.size() != 2) {
        return vector;
    }

    if (tmp0[0] != "bytes") {
        return vector;
    }

    auto tmp1 = text::StringBuilder::split(tmp0[1], ",");
    if (tmp1.empty()) {
        return vector;
    }

    if (tmp1.size() > 16) {
        return vector;
    }

    for (decltype(auto) item: tmp1) {
        auto tmp2 = sese::text::StringBuilder::split(item, "-");
        if (tmp2.size() == 1) {
            char *end_ptr;
            auto begin = std::strtol(tmp2[0].c_str(), &end_ptr, 10);
            if (begin > total - 1) {
                vector.clear();
                break;
            }
            vector.emplace_back(begin, total - begin);
        } else if (tmp2.size() == 2) {
            char *end_ptr;
            auto begin = std::strtol(tmp2[0].c_str(), &end_ptr, 10);
            auto end = std::strtol(tmp2[1].c_str(), &end_ptr, 10);
            if (begin > total - 1 || end > total - 1) {
                vector.clear();
                break;
            }
            if (begin > end) {
                vector.clear();
                break;
            }
            vector.emplace_back(begin, end - begin + 1);
        } else {
            vector.clear();
            break;
        }
    }

    return vector;
}

std::string sese::net::http::Range::toString(size_t total) const noexcept {
    return "bytes " + std::to_string(begin) + "-" + std::to_string(begin + len - 1) + "/" + std::to_string(total);
}

size_t sese::net::http::Range::toStringLength(size_t total) const noexcept {
    size_t length = strlen("bytes ");
    length += number2StringLength(begin, 10);
    length += strlen("-");
    length += number2StringLength(begin + len - 1, 10);
    length += strlen("/");
    length += number2StringLength(total, 10);
    return length;
}
