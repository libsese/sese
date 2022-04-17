#include "Random.h"
#include "system/CpuInfo.h"

#ifdef _WIN32
#pragma warning(disable : 4293)
#endif

namespace sese {

    const uint64_t Random::multiplier = 25214903917L;
    const uint64_t Random::addend = 11L;
    const uint64_t Random::mask = (1L << 48) - 1;
    uint64_t Random::seed = CpuInfo::RDTSC();

    /**
     * @brief 数据分割用结构体
     */
    struct LongLongSplitter {
        uint32_t low;
        [[maybe_unused]] uint32_t high;
    };

    uint64_t Random::next() {
        auto unit = (LongLongSplitter *) &seed;
        seed = (unit->low * multiplier + addend) & mask;
        return seed ^ CpuInfo::RDTSC();
    }

}// namespace sese