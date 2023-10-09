/// \file TimerableService_V2.h
/// \author kaoru
/// \date 2023年9月19日
/// \brief 可定时服务
/// \version 0.2.0

#pragma once

#include <sese/event/Event.h>
#include <sese/util/TimeWheel.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4275)
#endif

namespace sese::service {

/// 超时事件结构
struct TimeoutEvent_V2 {
    TimeoutEvent *event{nullptr};
    /// 额外数据
    void *data{nullptr};
};

/// 可定时服务
class API TimerableService_V2 : public event::EventLoop {
public:
    /// 分发事件
    /// \param timeout 超时时间
    void dispatch(uint32_t timeout) override;

    /// 超时回调函数
    /// \param event 事件
    virtual void onTimeout(TimeoutEvent_V2 *event);

    /// 设置超时事件
    /// \param seconds 超时事件
    /// \param data 附加数据
    /// \return 超时事件结构
    TimeoutEvent_V2 *setTimeoutEvent(int64_t seconds, void *data);

    /// 取消并释放超时事件
    /// \param event 超时事件
    void cancelTimeoutEvent(TimeoutEvent_V2 *event);

protected:
    /// 时间轮
    TimeWheel timeWheel{};
};

} // namespace sese::service
