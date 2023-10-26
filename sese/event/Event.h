/**
 * @file Event.h
 * @brief 网络事件结构关联
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