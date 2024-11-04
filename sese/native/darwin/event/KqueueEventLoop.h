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
 * @file KqueueEventLoop.h
 * @brief kqueue 事件循环
 * @author kaoru
 */

#pragma once

#include <sese/event/BaseEventLoop.h>
#include <sese/native/darwin/event/KqueueEvent.h>

#include <atomic>

namespace sese::event {
/// kqueue 事件循环
class KqueueEventLoop : public BaseEventLoop {
public:
    bool init() override;

    ~KqueueEventLoop() override;

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

    //protected:
    //    bool addNativeEvent(int fd, uint32_t ev, void *data) const;
    //
    //    bool delNativeEvent(int fd, uint32_t ev, void *data) const;

protected:
    int listenFd{-1};
    BaseEvent *listenEvent{nullptr};
    int kqueue{-1};
};

} // namespace sese::event