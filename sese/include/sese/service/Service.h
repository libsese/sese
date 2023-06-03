#pragma once

#include "sese/Config.h"

#ifdef SESE_PLATFORM_LINUX

#include "sese/service/SystemBalanceLoader.h"

namespace sese::service {
    using BalanceLoader = SystemBalanceLoader;
    using Service = sese::event::EventLoop;
}

#endif