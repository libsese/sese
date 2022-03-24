#pragma once
#include "Config.h"

namespace sese {
    class API TimeSpan {
    public:
        static const TimeSpan Zero;

    public:
        TimeSpan(int32_t days, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds, int32_t microseconds) noexcept;
        TimeSpan(int64_t timestamp, int64_t u_sec) noexcept;

        [[nodiscard]] int32_t getDays() const noexcept {
            return days;
        }

        [[nodiscard]] int32_t getHours() const noexcept {
            return hours;
        }

        [[nodiscard]] int32_t getMinutes() const noexcept {
            return minutes;
        }

        [[nodiscard]] int32_t getSeconds() const noexcept {
            return seconds;
        }

        [[nodiscard]] int32_t getMilliseconds() const noexcept {
            return milliseconds;
        }

        [[nodiscard]] int32_t getMicroseconds() const noexcept {
            return microseconds;
        }

        [[nodiscard]] int64_t getTimestamp() const noexcept {
            return timestamp;
        }

        [[nodiscard]] int64_t getUSecond() const noexcept {
            return u_sec;
        }

    private:
        int32_t days = 0;
        int32_t hours = 0;
        int32_t minutes = 0;
        int32_t seconds = 0;
        int32_t milliseconds = 0;
        int32_t microseconds = 0;
        int64_t timestamp = 0;
        int64_t u_sec = 0;
    };
}// namespace sese