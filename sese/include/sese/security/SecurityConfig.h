#pragma once
#include "sese/util/Initializer.h"

namespace sese::security {
    class API SecurityInitTask final : public InitiateTask {
    public:
        SecurityInitTask() : InitiateTask(__FUNCTION__) {}

        int32_t init() noexcept override;
    };
}