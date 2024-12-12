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

/// \file TimerableService_V2.h
/// \author kaoru
/// \date September 19, 2023
/// \brief Timerable Service
/// \version 0.2.0

#pragma once

#include <sese/event/Event.h>
#include <sese/util/TimeWheel.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4275)
#endif

namespace sese::service::v2 {

/// Timeout event structure
struct TimeoutEvent {
    sese::TimeoutEvent *event{nullptr};
    /// Additional data
    void *data{nullptr};
};

/// Timerable Service
class TimerableService : public event::EventLoop {
public:
    /// Dispatch events
    /// \param timeout Timeout duration
    void dispatch(uint32_t timeout) override;

    /// Timeout callback function
    /// \param event Event
    virtual void onTimeout(v2::TimeoutEvent *event);

    /// Set a timeout event
    /// \param seconds Timeout duration
    /// \param data Additional data
    /// \return Timeout event structure
    v2::TimeoutEvent *setTimeoutEvent(int64_t seconds, void *data);

    /// Cancel and free the timeout event
    /// \param event Timeout event
    void cancelTimeoutEvent(v2::TimeoutEvent *event);

private:
    /// Time wheel
    TimeWheel timeWheel{};
};

} // namespace sese::service
