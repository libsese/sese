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
}// namespace sese::event