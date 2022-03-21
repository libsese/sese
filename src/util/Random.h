#pragma once
#include "Config.h"

namespace sese {
    class API Random {
    private:
        static const uint64_t multiplier;
        static const uint64_t addend;
        static const uint64_t mask;
        static uint64_t seed;

    public:
        static uint64_t next();
    };
}// namespace sese