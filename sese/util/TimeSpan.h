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
 * @file TimeSpan.h
 * @brief Time Interval Class
 * @author kaoru
 * @date March 28, 2022
 * @version 0.2
 */

#pragma once

#include "sese/Config.h"
#include "sese/text/Format.h"


#ifdef _WIN32
#pragma warning(disable : 4819)
#endif

namespace sese {
/**
 * @brief Time Interval Class
 */
class TimeSpan {
public:
    /**
     * @brief Zero time interval
     */
    static const TimeSpan ZERO;

public:
    TimeSpan(int32_t days, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds, int32_t microseconds) noexcept;
    explicit TimeSpan(uint64_t timestamp) noexcept;

    /**
     * Get the number of days represented by the current structure
     * @return Number of days
     */
    [[nodiscard]] int32_t getDays() const noexcept;

    /**
     * Get the number of hours represented by the current structure
     * @return Number of hours
     */
    [[nodiscard]] int32_t getHours() const noexcept;

    /**
     * Get the number of minutes represented by the current structure
     * @return Number of minutes
     */
    [[nodiscard]] int32_t getMinutes() const noexcept;

    /**
     * Get the number of seconds represented by the current structure
     * @return Number of seconds
     */
    [[nodiscard]] int32_t getSeconds() const noexcept;

    /**
     * Get the number of milliseconds represented by the current structure
     * @return Number of milliseconds
     */
    [[nodiscard]] int32_t getMilliseconds() const noexcept;

    /**
     * Get the number of microseconds represented by the current structure
     * @return Number of microseconds
     */
    [[nodiscard]] int32_t getMicroseconds() const noexcept;

    /**
     * Get the total number of days represented by the current structure
     * @return Total number of days
     */
    [[nodiscard]] double getTotalDays() const noexcept;

    /**
     * Get the total number of hours represented by the current structure
     * @return Total number of hours
     */
    [[nodiscard]] double getTotalHours() const noexcept;

    /**
     * Get the total number of minutes represented by the current structure
     * @return Total number of minutes
     */
    [[nodiscard]] double getTotalMinutes() const noexcept;

    /**
     * Get the total number of seconds represented by the current structure
     * @return Total number of seconds
     */
    [[nodiscard]] double getTotalSeconds() const noexcept;

    /**
     * Get the total number of milliseconds represented by the current structure
     * @return Total number of milliseconds
     */
    [[nodiscard]] double getTotalMilliseconds() const noexcept;

    /**
     * Get the total number of microseconds represented by the current structure
     * @return Total number of microseconds
     */
    [[nodiscard]] uint64_t getTotalMicroseconds() const noexcept;

    /**
     * Get the timestamp with microseconds as the unit of time
     * @return Timestamp
     */
    [[nodiscard]] uint64_t getTimestamp() const noexcept { return timestamp; }

private:
    /// Timestamp in microseconds
    uint64_t timestamp = 0;
};
} // namespace sese
