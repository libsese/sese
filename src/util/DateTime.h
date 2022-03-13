#pragma once
#include "Config.h"
#include "TimeSpan.h"
#include <memory>

namespace sese {
    class API DateTime {
    public:
        typedef std::shared_ptr<DateTime> Ptr;

        static DateTime::Ptr now(int utc = DEFAULT_TIME_ZONE) noexcept;

        explicit DateTime(long timestamp, int utc = DEFAULT_TIME_ZONE) noexcept;

        DateTime(long timestamp, int milliseconds, int microseconds, int utc = DEFAULT_TIME_ZONE) noexcept;

    public:
        [[nodiscard]] bool isLeapYear() const noexcept { return this->isLeap; }
        [[nodiscard]] int getYears() const noexcept { return this->years; }
        [[nodiscard]] int getMonths() const noexcept { return this->months; }
        [[nodiscard]] int getDays() const noexcept { return this->days; }
        [[nodiscard]] int getHours() const noexcept { return this->hours; }
        [[nodiscard]] int getMinutes() const noexcept { return this->minutes; }
        [[nodiscard]] int getSeconds() const noexcept { return this->seconds; }
        [[nodiscard]] int getMilliseconds() const noexcept { return this->milliseconds; }
        [[nodiscard]] int getMicroseconds() const noexcept { return this->microseconds; }
        [[nodiscard]] long getTimestamp() const noexcept { return this->timestamp; }
        [[nodiscard]] int getUTC() const noexcept { return this->utc; }

    public:
        TimeSpan operator-(const DateTime &dateTime) const noexcept;
        DateTime operator-(const TimeSpan &timeSpan) const noexcept;
        DateTime operator+(const TimeSpan &timeSpan) const noexcept;
        [[nodiscard]] int compareTo(const DateTime &dateTime) const noexcept;
        [[nodiscard]] int unclearCompareTo(const DateTime &dateTime) const noexcept;

    private:
        bool isLeap = false;
        int years = 1970;
        int months = 1;
        int days = 1;
        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        int milliseconds = 0;
        int microseconds = 0;
        long timestamp = 0;
        int utc = 0;
    };
}// namespace sese