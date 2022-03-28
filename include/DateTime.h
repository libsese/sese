/**
 * @file DateTime.h
 * @author kaoru
 * @brief 日期时间类
 * @date 2022年03月28日
 */
#pragma once
#include "Config.h"
#include "TimeSpan.h"
#include <memory>

/**
 * 通用的时间 API
 * @param tp timeval 结构体
 * @param tzp timezone 结构体
 * @return 成功（0）， 失败（-1）
 */
inline int32_t getTimeOfDate(struct timeval *tp, struct timezone *tzp = nullptr);

namespace sese {

    /**
     * @brief 日期时间类
     */
    class API DateTime {
    public:
        typedef std::shared_ptr<DateTime> Ptr;

        static DateTime::Ptr now(int32_t utc = TIME_DEFAULT_ZONE) noexcept;

        explicit DateTime(int64_t timestamp, int32_t utc = TIME_DEFAULT_ZONE) noexcept;

        DateTime(int64_t timestamp, int64_t u_sec, int32_t utc = TIME_DEFAULT_ZONE) noexcept;

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
        [[nodiscard]] int64_t getTimestamp() const noexcept { return this->timestamp; }
        [[nodiscard]] int64_t getUSecond() const noexcept { return this->u_sec; }

    public:
        TimeSpan operator-(const DateTime &dateTime) const noexcept;
        DateTime operator-(const TimeSpan &timeSpan) const noexcept;
        DateTime operator+(const TimeSpan &timeSpan) const noexcept;
        /// 精细比较
        [[nodiscard]] int32_t compareTo(const DateTime &dateTime) const noexcept;
        /// 粗略比较
        [[nodiscard]] int32_t unclearCompareTo(const DateTime &dateTime) const noexcept;

    private:
        bool isLeap = false;
        int32_t years = 1970;
        int32_t months = 1;
        int32_t days = 1;
        int32_t hours = 0;
        int32_t minutes = 0;
        int32_t seconds = 0;
        int32_t milliseconds = 0;
        int32_t microseconds = 0;
        int32_t utc = 0;
        int32_t dayofweek = 0;
        int64_t timestamp = 0;
        int64_t u_sec = 0;
    };
}// namespace sese