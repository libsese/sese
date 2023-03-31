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
#ifdef SESE_PLATFORM_WINDOWS
        // CASE: SESE_PLATFORM_WINDOWS && SESE_ARCH_X64
        return __rdtsc();
#else
        // CASE: !SESE_PLATFORM_WINDOWS && SESE_ARCH_X64
        uint64_t value;
        __asm__ __volatile__("rdtsc" : "=A"(value));
        return value;
#endif
#else
        // CASE: SESE_ARCH_ARM64
        // 不再返回 cntfrq_el0 的原因是，此调用在 ARM 会进入内核态
        // 反复调用可能导致性能下降，x86 则不存在这个问题
        // uint64_t value;
        // __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (value));
        // return value;
        return 0;
#endif
    }

}// namespace sese