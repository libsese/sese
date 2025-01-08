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
 * @file WSAEventLoop.h
 * @brief WSAEventSelect event loop
 * @author kaoru
 */

#pragma once

#include <sese/internal/win/net/event/WSAEvent.h>
#include <sese/internal/win/net/event/WSAEventConvert.h>
#include <sese/net/event/BaseEventLoop.h>

#include <atomic>

#define MAX_EVENT_SIZE 64

namespace sese::event {
/// WSAEventSelect event loop
class WSAEventLoop : public BaseEventLoop {
public:
    ~WSAEventLoop() override;

    bool init() override;

    void dispatch(uint32_t timeout) override;

    void onAccept(int fd) override;

    void onRead(BaseEvent *event) override;

    void onWrite(BaseEvent *event) override;

    void onError(BaseEvent *event) override;

    void onClose(BaseEvent *event) override;

    BaseEvent *createEvent(int fd, unsigned int events, void *data) override;

    void freeEvent(BaseEvent *event) override;

    bool setEvent(BaseEvent *event) override;

    void setListenFd(int fd) override;

protected:
    int listenFd{-1};
    WSAEvent *listenEvent{nullptr};

    void *wsaEvent{nullptr};
    WSAEventConvert convert;

    unsigned long numbers = 0;
    unsigned long long sockets[MAX_EVENT_SIZE]{};
    void *wsaEvents[MAX_EVENT_SIZE]{};
    // Here, the lifecycle should be the responsibility of the user
    WSAEvent *events[MAX_EVENT_SIZE]{};
};

} // namespace sese::event

#undef MAX_EVENT_SIZE