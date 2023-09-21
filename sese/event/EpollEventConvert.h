#pragma once

#include "sese/event/BaseEventConvert.h"

namespace sese {
    namespace event {
        class EpollEventConvert : public BaseEventConvert {
        public:
            unsigned int fromNativeEvent(int event) override;

            int toNativeEvent(unsigned int event) override;
        };
    }
}