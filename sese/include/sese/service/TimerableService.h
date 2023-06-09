/// \file TimerableService.h
/// \author kaoru
/// \date 2023年6月9日
/// \brief 可定时服务
/// \version 0.1.0

#pragma once

#include "sese/event/Event.h"

#include <list>
#include <map>

namespace sese::service {

    /// 超时事件结构
    struct TimeoutEvent {
        /// 文件描述符
        int fd{0};
        /// 预期的时间戳
        uint64_t exceptTimestamp{0};
        /// 额外的数据
        void *data{nullptr};
    };

    /// 可定时服务
    class TimerableService : public event::EventLoop {
    public:
        ~TimerableService() override;

        bool init() override;

        void dispatch(uint32_t timeout) override;

    public:
        /// 当超时事件发生时触发
        /// \param timeoutEvent
        virtual void onTimeout(TimeoutEvent *timeoutEvent);

        /// 新建一个超时事件
        /// \note 每一个文件描述符只能对应一个超时事件，<p>
        ///       文件描述符重复将会导致原有超时事件被覆盖，<p>
        ///       通常文件描述符指的是套接字文件描述符。<p>
        ///       需要自定义与套接字无关的超时事件时，<p>
        ///       可以将文件描述符设置为小于 -1 的负数，<p>
        ///       超时服务不在乎文件描述符的符号。
        /// \param fd 文件描述符
        /// \param data 额外数据
        /// \param seconds 超时时间，单位是秒
        /// \return 超时事件结构
        TimeoutEvent *createTimeoutEvent(int fd, void *data, uint64_t seconds);

        /// 重设当前的超时事件，原本的事件将被取消并覆盖
        /// \see createTimoutEvent
        /// \param timeoutEvent 超时事件结构
        /// \param seconds 超时时间，单位是秒
        void setTimeoutEvent(TimeoutEvent *timeoutEvent, uint64_t seconds);

        /// 通过文件描述符获取超时事件结构
        /// \param fd 文件描述符
        /// \retval nullptr 对应的超时事件不存在
        /// \return 对应的超时事件结构
        TimeoutEvent *getTimeoutEventByFd(int fd);

        /// 取消当前的超时事件
        /// \param timeoutEvent 超时事件结构
        void cancelTimeoutEvent(TimeoutEvent *timeoutEvent);

        /// 释放当前的超时事件结构
        /// \param timeoutEvent 超时事件结构
        void freeTimeoutEvent(TimeoutEvent *timeoutEvent);

    private:
        uint64_t startTimestamp{0};

        std::map<int, TimeoutEvent *> timeoutMap;
        std::list<TimeoutEvent *> timeoutTable[60]{};
    };

}// namespace sese::service