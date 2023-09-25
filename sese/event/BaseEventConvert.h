#pragma once

#include "sese/event/BaseEvent.h"

namespace sese::event {
    class BaseEventConvert {
    public:
        virtual ~BaseEventConvert() = default;

        virtual unsigned int fromNativeEvent(int event) = 0;

        virtual int toNativeEvent(unsigned int event) = 0;
    };
}// namespace sese::event