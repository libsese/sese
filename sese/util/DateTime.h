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
 * @file DateTime.h
 * @author kaoru
 * @brief 日期时间类
 * @date 2022年03月28日
 */
#pragma once

#include "sese/Config.h"
#include "sese/util/TimeSpan.h"
#include "sese/text/DateTimeFormatter.h"

#ifdef _WIN32
#pragma warning(disable : 4819)
#endif

// /**
//  * 通用的时间 API
//  * @param tp timeval 结构体
//  * @param tzp timezone 结构体
//  * @return 成功（0）， 失败（-1）
//  */
// inline int32_t getTimeOfDate(struct timeval *tp, struct timezone *tzp = nullptr);

namespace sese {

/**
 * @brief 日期时间类
 */
class DateTime {
public:
    using Ptr = std::unique_ptr<DateTime>;

    /**
     * @brief 日期创建策略
     */
    enum class Policy {
        /// 仅仅创建时间戳而不解析为人类可阅读的时间格式
        ONLY_CREATE,
        /// 创建时间戳的同时解析成可阅读的格式
        FORMAT
    };

    static DateTime now(int32_t utc = TIME_DEFAULT_ZONE, Policy policy = Policy::FORMAT) noexcept;

    static DateTime::Ptr nowPtr(int32_t utc = TIME_DEFAULT_ZONE, Policy policy = Policy::FORMAT) noexcept;

    explicit DateTime() noexcept = default;

    explicit DateTime(uint64_t timestamp, int32_t utc = TIME_DEFAULT_ZONE, Policy policy = Policy::FORMAT) noexcept;

public:
    [[nodiscard]] bool isLeapYear() const noexcept { return this->isLeap; }
    [[nodiscard]] int32_t getYears() const noexcept { return this->years; }
    [[nodiscard]] int32_t getMonths() const noexcept { return this->months; }
    [[nodiscard]] int32_t getDays() const noexcept { return this->days; }
    [[nodiscard]] int32_t getHours() const noexcept { return this->hours; }
    [[nodiscard]] int32_t getMinutes() const noexcept { return this->minutes; }
    [[nodiscard]] int32_t getSeconds() const noexcept { return this->seconds; }
    [[nodiscard]] int32_t getMilliseconds() const noexcept { return this->milliseconds; }
    [[nodiscard]] int32_t getMicroseconds() const noexcept { return this->microseconds; }
    [[nodiscard]] int32_t getUTC() const noexcept { return this->utc; }
    [[nodiscard]] int32_t getDayOfWeek() const noexcept { return this->dayofweek; }
    [[nodiscard]] int32_t getDayOfYear() const noexcept { return this->dayofyear; }
    [[nodiscard]] uint64_t getTimestamp() const noexcept { return this->timestamp; }

public:
    TimeSpan operator-(const DateTime &date_time) const noexcept;
    DateTime operator-(const TimeSpan &time_span) const noexcept;
    DateTime operator+(const TimeSpan &time_span) const noexcept;
    /// 精细比较
    [[nodiscard]] int32_t compareTo(const DateTime &date_time) const noexcept;
    /// 粗略比较
    [[nodiscard]] int32_t unclearCompareTo(const DateTime &date_time) const noexcept;

private:
    // 初次计算的数据
    int32_t years = 1970;
    int32_t months = 1;
    int32_t days = 1;
    int32_t hours = 0;
    int32_t minutes = 0;
    int32_t seconds = 0;
    int32_t dayofweek = 0;
    int32_t dayofyear = 0;

    bool isLeap = false;
    int32_t milliseconds = 0;
    int32_t microseconds = 0;

    // 核心数据
    int32_t utc = 0;
    uint64_t timestamp = 0;
};

template<>
struct text::overload::Formatter<DateTime> {
    std::string datetime_pattern = "yyyy-MM-dd HH:mm:ss";

    bool parse(const std::string &pattern) {
        datetime_pattern = pattern;
        return true;
    }

    void format(FmtCtx &ctx, const DateTime &datetime) const {
        ctx.builder << DateTimeFormatter::format(datetime, datetime_pattern);
    }
};

} // namespace sese