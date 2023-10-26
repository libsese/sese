/**
 * @file WSAEvent.h
 * @brief WSAEventSelect 事件
 * @author kaoru
 */

#pragma once

#include "sese/event/BaseEvent.h"

namespace sese::event {
/// WSAEventSelect 事件
class WSAEvent : public BaseEvent {
public:
    void *wsaEvent{nullptr};
};
} // namespace sese::event