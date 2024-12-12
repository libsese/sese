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

/**
* @file Range.h
* @brief HTTP Content Range Class
* @author kaoru
* @version 0.1
* @date September 13, 2023
*/

#pragma once

#include <sese/Config.h>

#include <vector>

namespace sese::net::http {

/// HTTP Content Range Class
struct Range {
    size_t begin = 0;
    size_t len = 0;

    Range(size_t begin, size_t len) noexcept;

    /** @brief Parse Range field
     *  @param str Range value
     *  @verbatim
        bytes=0-1023
        bytes=0-50, 100-150, 200-
     *  @endverbatim
     *  @param total Total size of the range
     *  @return Range
     **/
    static std::vector<Range> parse(const std::string &str, size_t total) noexcept;

    [[nodiscard]] std::string toString(size_t total) const noexcept;

    [[nodiscard]] size_t toStringLength(size_t total) const noexcept;
};

} // namespace sese::net::http
