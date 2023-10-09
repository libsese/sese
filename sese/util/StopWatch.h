/**
 * @file StopWatch.h
 * @brief 停表类
 * @author kaoru
 * @version 0.1
 */

#pragma once

#include <sese/Config.h>
#include <sese/util/DateTime.h>

namespace sese {

/// 停表类
class API StopWatch {
public:
    StopWatch();

    /**
     * 按下停表并返回距离上一次停止的时间间隔
     * @return 时间间隔
     */
    TimeSpan stop();

    /**
     * 重置停表
     */
    void reset();

protected:
    DateTime lastStopTime;
};
} // namespace sese
