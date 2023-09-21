#pragma once

#include "sese/event/BaseEvent.h"

namespace sese {
    namespace event {
        class KqueueEvent;
    }
}

class sese::event::KqueueEvent : public BaseEvent {
public:
    unsigned int oldEvents{EVENT_NULL};
};