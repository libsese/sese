/// \file BalanceLoader.h
/// \author kaoru
/// \date 2023年6月9日
/// \brief 均衡加载器

#pragma once

#include "sese/Config.h"

#ifdef SESE_PLATFORM_LINUX

#include "sese/service/SystemBalanceLoader.h"

namespace sese::service {
using BalanceLoader = SystemBalanceLoader;
using Service = sese::event::EventLoop;
} // namespace sese::service

#else

#include "sese/service/UserBalanceLoader.h"

namespace sese::service {
using BalanceLoader = UserBalanceLoader;
using Service = sese::event::EventLoop;
} // namespace sese::service

#endif