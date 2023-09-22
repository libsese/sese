#pragma once

#include "sese/event/BaseEvent.h"

namespace sese::event {
    class WSAEvent : public BaseEvent {
    public:
        void *wsaEvent{nullptr};
    };
}// namespace sese::event