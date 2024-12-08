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
 * @file DateTimeFormatter.h
 * @date March 28, 2022
 * @author kaoru
 * @brief Date time formatter class
 */

#pragma once

#include "sese/Config.h"
#include "sese/text/StringBuilder.h"

#include <map>
#include <array>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
namespace sese {

class DateTime;

namespace text {

    /**
     * @brief Date time formatter class
     * @verbatim
     Time format specifiers
     d    Day of the month (1-31)
     dd   Day of the month (01-31)
     ddd  Abbreviated day of the week (Mon)
     dddd Full name of the day of the week (Monday)
     m    Minute (0-59)
     mm   Minute (00-59)
     M    Month (1-12)
     MM   Month (01-12)
     MMM  Abbreviated month (Jun)
     MMMM Full name of the month (June)
     y    Year (0-99)
     yy   Year (00-99)
     yyy  Year (000-999)
     yyyy Year (0000-9999)
     z    Offset from UTC
     h    12-hour format (0-11)
     hh   12-hour format (00-11)
     H    24-hour format (0-23)
     HH   24-hour format (00-23)
     t    AM / PM - can be used with the 12-hour format
     s    Second (0-59)
     ss   Second (00-59)
     f    Millisecond (0-999)
     ff   Millisecond (000-999)
     fff  Microsecond (0-999)
     ffff Microsecond (000-999)
     %    Escape character
     @endverbatim
     */
    class DateTimeFormatter {
    public:
        static std::string format(const DateTime &date_time, const std::string &pattern = TIME_DEFAULT_PATTERN);

        static std::string format(const std::unique_ptr<DateTime> &date_time, const std::string &pattern = TIME_DEFAULT_PATTERN);

        /** Convert Greenwich Mean Time to timestamp
         * \param text Greenwich Mean Time text
         * \verbatim
           "Tue, 17 Oct 2023 15:41:22 GMT"
           "Thu, 31-Dec-37 23:55:55 GMT"
           \endverbatim
         * \return Timestamp
         **/
        static uint64_t parseFromGreenwich(const std::string &text);

        /** Convert ISO 8601 time to timestamp
         * \param text ISO 8601 time text
         * \verbatim
           "2023-06-20"
           "2023-06-20 16:46:55"
           "2023-06-20T16:46:55Z"
           "2023-06-20 16:46:55 +8"
           "2023-06-20T16:46:55+00:00"
           \endverbatim
         * \return Timestamp
         */
        static uint64_t parseFromISO8601(const std::string &text);

    protected:
        static int mon2number(const std::string &text);

        static const std::map<std::string, uint8_t> MON_MAP;
        static const std::array<std::string, 12> MON_ARRAY;

        static const std::map<std::string, uint8_t> MONTH_MAP;
        static const std::array<std::string, 12> MONTH_ARRAY;

        static const std::array<std::string, 7> WK_DAY;
        static const std::array<std::string, 7> WEEK_DAY;
    };
} // namespace text
} // namespace sese