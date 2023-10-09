/**
 * @file TimeSpan.h
 * @brief 时间间隔类
 * @author kaoru
 * @date 2022年3月28日
 * @version 0.2
 */

#pragma once

#include "sese/Config.h"

#ifdef _WIN32
#pragma warning(disable : 4819)
#endif

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
    explicit TimeSpan(uint64_t timestamp) noexcept;

    /**
     * 获取当前结构所表示的时间间隔的天数
     * @return 天数
     */
    [[nodiscard]] int32_t getDays() const noexcept;

    /**
     * 获取当前结构所表示的时间间隔的小时数
     * @return 小时数
     */
    [[nodiscard]] int32_t getHours() const noexcept;

    /**
     * 获取当前结构所表示的时间间隔的分钟数
     * @return 分钟数
     */
    [[nodiscard]] int32_t getMinutes() const noexcept;

    /**
     * 获取当前结构所表示的时间间隔的秒数
     * @return 秒数
     */
    [[nodiscard]] int32_t getSeconds() const noexcept;

    /**
     * 获取当前结构所表示的时间间隔的毫秒数
     * @return 毫秒数
     */
    [[nodiscard]] int32_t getMilliseconds() const noexcept;

    /**
     * 获取当前结构所表示的时间间隔的微秒数
     * @return 微秒数
     */
    [[nodiscard]] int32_t getMicroseconds() const noexcept;

    /**
     * 获取当前结构以总和形式表示的天数
     * @return 天数
     */
    [[nodiscard]] double getTotalDays() const noexcept;

    /**
     * 获取当前结构以总和形式表示的小时数
     * @return 小时数
     */
    [[nodiscard]] double getTotalHours() const noexcept;

    /**
     * 获取当前结构以总和形式表示的分钟数
     * @return 分钟数
     */
    [[nodiscard]] double getTotalMinutes() const noexcept;

    /**
     * 获取当前结构以总和形式表示的秒数
     * @return 秒数
     */
    [[nodiscard]] double getTotalSeconds() const noexcept;

    /**
     * 获取当前结构以总和形式表示的毫秒数
     * @return 毫秒数
     */
    [[nodiscard]] double getTotalMilliseconds() const noexcept;

    /**
     * 获取当前结构以总和形式表示的微秒数
     * @return 微秒数
     */
    [[nodiscard]] uint64_t getTotalMicroseconds() const noexcept;

    /**
     * 获取以微妙为时间单位的时间戳
     * @return 时间戳
     */
    [[nodiscard]] uint64_t getTimestamp() const noexcept { return timestamp; }

private:
    /// 以微秒为单位的时间戳
    uint64_t timestamp = 0;
};
} // namespace sese