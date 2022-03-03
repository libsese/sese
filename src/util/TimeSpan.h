#pragma once

namespace sese {
    class TimeSpan {
    public:
        static const TimeSpan Zero;

    public:
        TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds, int microseconds) noexcept;
        TimeSpan(long timestamp, int milliseconds, int microseconds) noexcept;

        [[nodiscard]] int getDays() const noexcept {
            return days;
        }

        [[nodiscard]] int getHours() const noexcept {
            return hours;
        }

        [[nodiscard]] int getMinutes() const noexcept {
            return minutes;
        }

        [[nodiscard]] int getSeconds() const noexcept {
            return seconds;
        }

        [[nodiscard]] int getMilliseconds() const noexcept {
            return milliseconds;
        }

        [[nodiscard]] int getMicroseconds() const noexcept {
            return microseconds;
        }

        [[nodiscard]] long getTimestamp() const noexcept {
            return timestamp;
        }

    private:
        int days = 0;
        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        int milliseconds = 0;
        int microseconds = 0;
        long timestamp = 0;
    };
}// namespace sese