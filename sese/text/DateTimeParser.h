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

/// \file DateTimeParser.h
/// \brief Date parser
/// \author kaoru
/// \date December 29, 2023

#pragma once

#include <sese/Config.h>
#include <sese/util/DateTime.h>

#include <optional>

namespace sese::text {

/// Date parser
class DateTimeParser {
public:
    /**
     * Attempt to restore the string to a data structure
     * \param format Time match format
     * \verbatim
     * yyyy Full year
     * MM   Full month
     * dd   Full day of the month
     * HH   24-hour format hour
     * mm   Full minute
     * ss   Full second
     * fff  Full millisecond
     * rrr  Full microsecond
     * %    Escape character
     * '*'  Word
     * z    UTC offset
     * \endverbatim
     * \param datetime Time string
     * \return Time data structure
     */
    static std::optional<DateTime> parse(const std::string &format, const std::string &datetime);

private:
    static int count(const char *string, int max);
};

} // namespace sese::text