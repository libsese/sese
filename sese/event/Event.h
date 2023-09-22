#pragma once

#if defined(__linux__)

#include "sese/native/Linux/event/EpollEvent.h"
#include "sese/native/Linux/event/EpollEventLoop.h"
#include "sese/native/Linux/event/EpollEventConvert.h"

namespace sese {
    using Event = BaseEvent;
    using EventLoop = EpollEventLoop;
    using EventConvert = EpollEventConvert;
}// namespace sese::event

#endif

#if defined(_WIN32)

#include "sese/native/Windows/event/WSAEvent.h"
#include "sese/native/Windows/event/WSAEventLoop.h"
#include "sese/native/Windows/event/WSAEventConvert.h"

namespace sese::event {
    using Event = BaseEvent;
    using EventLoop = WSAEventLoop;
    using EventConvert = WSAEventConvert;
}// namespace sese::event

#endif

#if defined(__APPLE__)

#include "sese/native/Darwin/event/KqueueEvent.h"
#include "sese/native/Darwin/event/KqueueEventLoop.h"

namespace sese::event {
    using Event = BaseEvent;
    using EventLoop = KqueueEventLoop;
}// namespace sese::event

#endif