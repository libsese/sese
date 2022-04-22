/**
 * @file TimeSpan.h
 * @brief 时间间隔类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "sese/Config.h"

namespace sese {
    /**
     * @brief 时间间隔类
     */
    class API TimeSpan {
    public:
        /**
         * @brief 零时间间隔
         */
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

        /**
         * 获取毫秒数
         * @return 毫秒
         */
        [[nodiscard]] int32_t getMilliseconds() const noexcept {
            return milliseconds;
        }

        /**
         * 获取微秒数
         * @return 微秒
         */
        [[nodiscard]] int32_t getMicroseconds() const noexcept {
            return microseconds;
        }

        /**
         * 获取以秒为时间单位的时间戳
         * @return 时间戳
         */
        [[nodiscard]] int64_t getTimestamp() const noexcept {
            return timestamp;
        }

        /**
         * 获取以微秒为单位的时间戳
         * @return 时间戳
         */
        [[nodiscard]] int64_t getUSecond() const noexcept {
            return u_sec;
        }

    private:
        /// 天
        int32_t days = 0;
        /// 时
        int32_t hours = 0;
        /// 分
        int32_t minutes = 0;
        /// 秒
        int32_t seconds = 0;
        /// 毫秒
        int32_t milliseconds = 0;
        /// 微秒
        int32_t microseconds = 0;
        /// 以秒为单位的时间戳
        int64_t timestamp = 0;
        /// 以微秒为单位的时间戳
        int64_t u_sec = 0;
    };
}// namespace sese