#pragma once
#include "Config.h"

namespace sese {
    class API Random {
    public:
        static int randInt();
        static int randIntBetween(int min, int max);

        static double randDouble();
    };
}// namespace sese