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
 * @file Event.h
 * @brief Network event structure association
 * @author kaoru
 */

#pragma once

#if defined(__linux__)

#include "sese/native/linux/event/EpollEvent.h"
#include "sese/native/linux/event/EpollEventLoop.h"
#include "sese/native/linux/event/EpollEventConvert.h"

namespace sese::event {
using Event = BaseEvent;
using EventLoop = EpollEventLoop;
using EventConvert = EpollEventConvert;
} // namespace sese::event

#endif

#if defined(_WIN32)

#include "sese/native/win/event/WSAEvent.h"
#include "sese/native/win/event/WSAEventLoop.h"
#include "sese/native/win/event/WSAEventConvert.h"

namespace sese::event {
using Event = BaseEvent;
using EventLoop = WSAEventLoop;
using EventConvert = WSAEventConvert;
} // namespace sese::event

#endif

#if defined(__APPLE__)

#include "sese/native/darwin/event/KqueueEvent.h"
#include "sese/native/darwin/event/KqueueEventLoop.h"

namespace sese::event {
using Event = BaseEvent;
using EventLoop = KqueueEventLoop;
} // namespace sese::event

#endif