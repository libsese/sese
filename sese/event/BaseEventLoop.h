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
 * @file BaseEventLoop.h
 * @brief 基础网络事件循环接口
 * @author kaoru
 */

#pragma once

#include <sese/event/BaseEvent.h>

#include <cstdint>

namespace sese::event {

/// 基础网络事件循环接口
class BaseEventLoop {
public:
    virtual bool init() = 0;

    virtual ~BaseEventLoop() = default;

    virtual void dispatch(uint32_t timeout) = 0;

    virtual void onAccept(int fd) = 0;

    virtual void onRead(BaseEvent *event) = 0;

    virtual void onWrite(BaseEvent *event) = 0;

    virtual void onError(BaseEvent *event) = 0;

    virtual void onClose(BaseEvent *event) = 0;

    virtual BaseEvent *createEvent(int fd, unsigned int events, void *data) = 0;

    virtual void freeEvent(BaseEvent *event) = 0;

    virtual bool setEvent(BaseEvent *event) = 0;

    virtual void setListenFd(int fd) = 0;

protected:
    /// 指示当前循环是否处理关闭事件
    bool handleClose = true;
};

} // namespace sese::event