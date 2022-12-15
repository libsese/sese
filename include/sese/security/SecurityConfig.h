#pragma once
#include <sese/Initializer.h>

namespace sese::security {
    class API SecurityInitTask : public InitiateTask {
    public:
        SecurityInitTask() : InitiateTask(__FUNCTION__) {}

        int32_t init() noexcept override;
    };
}