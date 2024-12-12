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
 * @file Timer.h
 * @brief Low-Precision Timer Class
 * @author kaoru
 * @date July 11, 2022
 * @version 0.3
 */
#pragma once

#include "sese/Config.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <functional>
#include <list>
#include <mutex>

namespace sese {

class Timer;
/// Timer Task
class TimerTask final : public std::enable_shared_from_this<TimerTask> {
public:
    friend class Timer;
    using Ptr = std::shared_ptr<TimerTask>;

    /// Cancel the current timer task
    void cancel() noexcept;

private:
    // Private constructor
    TimerTask() = default;
    // Corresponding sleep duration
    int64_t sleepTimestamp = 0;
    // Corresponding target timestamp
    int64_t targetTimestamp = 0;
    // Whether to repeat
    bool isRepeat = false;
    // Timer callback function
    std::function<void()> callback;
    // Cancel callback function
    std::function<void()> cancelCallback;
};

/// Low-Precision Timer Class
class Timer final : public std::enable_shared_from_this<Timer> {
public:
    using Ptr = std::shared_ptr<Timer>;

    /// Initialize a timer
    /// \param number Number of slots in the timer wheel
    static Timer::Ptr create(size_t number = TIMER_WHEEL_NUMBER) noexcept;
    /// Timer destructor
    ~Timer() noexcept;

    /// Set a delayed task
    /// \param callback Callback function
    /// \param relative_timestamp Delay duration
    /// \param is_repeat Whether to repeat
    /// \return Task handle
    TimerTask::Ptr delay(const std::function<void()> &callback, int64_t relative_timestamp, bool is_repeat = false) noexcept;
    /// Shutdown the timer and terminate the timer thread
    void shutdown() noexcept;

public:
    /// Private constructor
    Timer() = default;
    void loop() noexcept;
    static void cancelCallback(const std::weak_ptr<Timer> &weak_timer, const std::weak_ptr<TimerTask> &weak_task) noexcept;

    size_t number = 0;
    std::mutex mutex{};
    Thread::Ptr thread = nullptr;
    std::atomic_bool isShutdown = false;
    std::atomic<int64_t> currentTimestamp = 0;
    std::list<TimerTask::Ptr> *timerTasks = nullptr;
};
} // namespace sese
