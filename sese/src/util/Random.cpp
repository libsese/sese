#include "sese/util/Random.h"

#ifdef _WIN32
#pragma warning(disable : 4293)
#endif

namespace sese {

    const uint64_t Random::multiplier = 25214903917L;
    const uint64_t Random::addend = 11L;
    const uint64_t Random::mask = (1L << 48) - 1;
    uint64_t Random::seed = Random::noise();

    /**
     * @brief 数据分割用结构体
     */
    struct LongLongSplitter {
        uint32_t low;
        uint32_t high; // NOLINT
    };

    uint64_t Random::next() noexcept {
        auto unit = (LongLongSplitter *) &seed;
        seed = (unit->low * multiplier + addend) & mask;
        return seed ^ Random::noise();
    }

    uint64_t Random::noise() noexcept {
#ifdef SESE_ARCH_X64
#ifdef _WIN32
        return __rdtsc();
#else
        uint64_t value;
        __asm__ __volatile__("rdtsc" : "=A"(value));
        return value;
#endif
#else
        uint64_t value;
        __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (value));
        return value;
#endif
    }

}// namespace sese