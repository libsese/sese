#include "Random.h"
#include "system/CpuInfo.h"
#include <limits>

namespace sese {

    struct LongLongSpilter {
        unsigned low;
        [[maybe_unused]] unsigned high;
    };

    int Random::randInt() {
        auto rdtsc = CpuInfo::rdtsc();
        auto *reg = (LongLongSpilter *) &rdtsc;
        srand(reg->low);
        return rand();
    }

    int Random::randIntBetween(int min, int max) {
        auto rdtsc = CpuInfo::rdtsc();
        auto *reg = (LongLongSpilter *) &rdtsc;
        srand(reg->low);
        return min + rand() % (max - min);
    }

    double Random::randDouble() {
        auto rdtsc = CpuInfo::rdtsc();
        auto *reg = (LongLongSpilter *) &rdtsc;
        srand(reg->low);
        return (double) rand() / RAND_MAX;
    }

}// namespace sese