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
 * @file TimeWheel.h
 * @author kaoru
 * @brief 低精度时间轮算法
 * @version 0.2
 * @date 2023年9月15日
 */

#pragma once

#include <sese/Config.h>

#include <list>
#include <functional>

namespace sese {

/**
 * @brief 超时事件
 */
struct TimeoutEvent {
    /// 超时事件发生时的回调函数类型
    using Callback = std::function<void()>;

    /// 超时事件定时事件总长
    int64_t range{};
    /// 超时事件具体将会发生的时间
    int64_t target{};
    /// 超时事件发生后，是否进行下一次定时
    bool repeat{false};
    /// 超时事件发生时的回调函数
    Callback callback{};
};

/**
 * @brief 超时事件槽（时间轮轮片）
 */
struct TimeoutEventSlot {
    /// 存放于此的超时事件列表
    std::list<TimeoutEvent *> events{};
    /// 指向下一个超时事件槽
    TimeoutEventSlot *next{nullptr};
};

/**
 * @brief 时间轮
 */
class  TimeWheel {
public:
    TimeWheel();

    virtual ~TimeWheel();

    /// 添加一个超时事件
    /// \param callback 超时回调函数
    /// \param seconds 超时时间
    /// \param repeat 事件是否重复触发
    /// \return 超时事件
    TimeoutEvent *delay(
            const TimeoutEvent::Callback &callback,
            int64_t seconds,
            bool repeat = false
    );

    /// 取消一个超时事件，事件将变得不可用
    /// \param event 超时事件
    void cancel(TimeoutEvent *event);

    /// 检查当前是否有需要触发的超时事件，有则触发对应的回调函数并根据需要销毁事件
    void check();

protected:
    static int64_t getTimestamp();

    int64_t startTime{};
    int64_t lastCheckTime{};
    TimeoutEventSlot slots[60]{};
};

} // namespace sese