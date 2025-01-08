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
*  @file EpollEventLoop.h
*  @brief epoll event loop
*  @author kaoru
*/

#pragma once

#include "sese/net/event/BaseEvent.h"
#include "sese/net/event/BaseEventLoop.h"
#include "sese/internal/linux/net/event/EpollEvent.h"
#include "sese/internal/linux/net/event/EpollEventConvert.h"

namespace sese::event {
/// epoll event loop
class EpollEventLoop : public sese::event::BaseEventLoop {
public:
    bool init() override;

    ~EpollEventLoop() override;

    void dispatch(uint32_t timeout) override;

    void onAccept(int fd) override;

    void onRead(BaseEvent *event) override;

    void onWrite(BaseEvent *event) override;

    void onError(BaseEvent *event) override;

    void onClose(BaseEvent *event) override;

    BaseEvent *createEvent(int fd, unsigned int events, void *data) override;

    void freeEvent(BaseEvent *event) override;

    bool setEvent(BaseEvent *event) override;

    void setListenFd(int fd) override { listenFd = fd; }

protected:
    int listenFd{-1};
    BaseEvent *listenEvent{nullptr};

    int epoll{-1};
    EpollEventConvert convert;
};
} // namespace sese::event