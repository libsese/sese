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

/// \file TimerableService_V1.h
/// \author kaoru
/// \date June 9, 2023
/// \brief Timerable Service
/// \version 0.1.0

#pragma once

#include "sese/event/Event.h"

#include <list>
#include <map>

namespace sese::service::v1 {

/// Timeout event structure
struct TimeoutEvent {
    /// File descriptor
    int fd{0};
    /// Expected timestamp
    uint64_t exceptTimestamp{0};
    /// Extra data
    void *data{nullptr};
};

/// Timerable Service
class TimerableService : public event::EventLoop {
public:
    ~TimerableService() override;

    bool init() override;

    void dispatch(uint32_t timeout) override;

public:
    /// Triggered when a timeout event occurs
    /// \param timeout_event Timeout event
    virtual void onTimeout(TimeoutEvent *timeout_event);

    /// Create a timeout event
    /// \note Each file descriptor can correspond to only one timeout event. <p>
    ///       Repeating the file descriptor will cause the original timeout event to be overwritten. <p>
    ///       Typically, file descriptors refer to socket file descriptors. <p>
    ///       For custom timeout events unrelated to sockets, <p>
    ///       the file descriptor can be set to a negative number less than -1. <p>
    ///       The timeout service does not care about the sign of the file descriptor.
    /// \param fd File descriptor
    /// \param data Extra data
    /// \param seconds Timeout in seconds
    /// \return Timeout event structure
    TimeoutEvent *createTimeoutEvent(int fd, void *data, uint64_t seconds);

    /// Reset the current timeout event, the original event will be canceled and overwritten
    /// \see createTimoutEvent
    /// \param timeout_event Timeout event structure
    /// \param seconds Timeout in seconds
    void setTimeoutEvent(TimeoutEvent *timeout_event, uint64_t seconds);

    /// Get the timeout event structure by file descriptor
    /// \param fd File descriptor
    /// \retval nullptr The corresponding timeout event does not exist
    /// \return The corresponding timeout event structure
    TimeoutEvent *getTimeoutEventByFd(int fd);

    /// Cancel the current timeout event
    /// \param timeout_event Timeout event structure
    void cancelTimeoutEvent(TimeoutEvent *timeout_event);

    /// Release the current timeout event structure
    /// \param timeout_event Timeout event structure
    void freeTimeoutEvent(TimeoutEvent *timeout_event);

private:
    uint64_t startTimestamp{0};

    std::map<int, TimeoutEvent *> timeoutMap;
    std::list<TimeoutEvent *> timeoutTable[60]{};
};

} // namespace sese::service