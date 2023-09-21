#pragma once

#include "sese/event/BaseEvent.h"

namespace sese {
    namespace event {
        class BaseEventConvert;
    }
}

class sese::event::BaseEventConvert {
public:
    virtual ~BaseEventConvert() = default;

    virtual unsigned int fromNativeEvent(int event) = 0;

    virtual int toNativeEvent(unsigned int event) = 0;
};