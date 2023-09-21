#pragma once

#include "sese/event/BaseEvent.h"

namespace sese {
    namespace event {
        class WSAEvent : public BaseEvent {
        public:
            void *wsaEvent{nullptr};
        };
    }
}