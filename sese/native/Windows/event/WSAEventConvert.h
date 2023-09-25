#pragma once

#include "sese/event/BaseEventConvert.h"

namespace sese::event {
    class WSAEventConvert : public BaseEventConvert {
    public:
        unsigned int fromNativeEvent(int event) override;

        int toNativeEvent(unsigned int event) override;
    };
}// namespace sese::event