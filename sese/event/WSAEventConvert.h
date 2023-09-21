#pragma once

#include "sese/event/BaseEventConvert.h"

namespace sese {
    namespace event {
        class WSAEventConvert;
    }
}

class sese::event::WSAEventConvert : public BaseEventConvert {
public:
    unsigned int fromNativeEvent(int event) override;

    int toNativeEvent(unsigned int event) override;
};