/// \file TimerableService.h
/// \author kaoru
/// \date 2023年9月16日
/// \brief 可定时服务
/// \version 0.1.0

#pragma once

#include <sese/service/TimerableService_V1.h>
#include <sese/service/TimerableService_V2.h>

namespace sese::service {
/// 超时事件结构体
typedef v2::TimeoutEvent TimeoutEvent;

/// 可定时服务
typedef v2::TimerableService TimerableService;
} // namespace sese::service