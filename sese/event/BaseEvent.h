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
 * @file BaseEvent.h
 * @brief 网络事件基类
 * @author kaoru
 */

#pragma once

#define EVENT_NULL 0x0u
#define EVENT_READ 0x1u
#define EVENT_WRITE 0x2u
#define EVENT_ERROR 0x4u

namespace sese::event {
/// 网络事件基类
struct BaseEvent {
    virtual ~BaseEvent() = default;

    int fd{0};
    unsigned int events{EVENT_NULL};
    void *data{nullptr};
};
} // namespace sese::event