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
 * @brief Low-Precision Time Wheel Algorithm
 * @author kaoru
 * @version 0.2
 * @date September 15, 2023
 */

#pragma once

#include <sese/Config.h>

#include <list>
#include <functional>

namespace sese {

/**
 * @brief Timeout Event
 */
struct TimeoutEvent {
    /// Type of the callback function when a timeout event occurs
    using Callback = std::function<void()>;

    /// Total duration of the timeout event
    int64_t range{};
    /// Specific time the timeout event will occur
    int64_t target{};
    /// Whether to repeat the timeout event after it occurs
    bool repeat{false};
    /// Callback function when the timeout event occurs
    Callback callback{};
};

/**
 * @brief Timeout Event Slot (Time Wheel Slice)
 */
struct TimeoutEventSlot {
    /// List of timeout events stored here
    std::list<TimeoutEvent *> events{};
    /// Pointer to the next timeout event slot
    TimeoutEventSlot *next{nullptr};
};

/**
 * @brief Time Wheel
 */
class TimeWheel {
public:
    TimeWheel();

    virtual ~TimeWheel();

    /// Add a timeout event
    /// \param callback Timeout callback function
    /// \param seconds Timeout duration
    /// \param repeat Whether the event triggers repeatedly
    /// \return Timeout event
    TimeoutEvent *delay(
            const TimeoutEvent::Callback &callback,
            int64_t seconds,
            bool repeat = false
    );

    /// Cancel a timeout event, making it unavailable
    /// \param event Timeout event
    void cancel(TimeoutEvent *event);

    /// Check for any timeout events that need to be triggered, and trigger the corresponding callback functions and destroy the events as needed
    void check();

protected:
    static int64_t getTimestamp();

    int64_t startTime{};
    int64_t lastCheckTime{};
    TimeoutEventSlot slots[60]{};
};

} // namespace sese
