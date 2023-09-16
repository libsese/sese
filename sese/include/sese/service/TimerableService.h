/// \file TimerableService.h
/// \author kaoru
/// \date 2023年9月16日
/// \brief 可定时服务
/// \version 0.1.0

#pragma once

#include <sese/service/TimerableService_V1.h>

namespace sese::service {
    /// 超时事件结构体
    typedef TimeoutEvent_V1 TimeoutEvent;

    /// 可定时服务
    typedef TimerableService_V1 TimerableService;
}