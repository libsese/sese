#include "sese/util/Random.h"

#ifdef _WIN32
#pragma warning(disable : 4293)
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace sese {

const Random::ResultType Random::MULTIPLIER = 25214903917L;
const Random::ResultType Random::ADDEND = 11L;
const Random::ResultType Random::MASK = (1LL << 48) - 1;

/**
     * @brief 数据分割用结构体
     */
struct LongLongSplitter {
    uint32_t low;
    [[maybe_unused]] uint32_t high;
};

uint64_t Random::noise() noexcept {
#ifdef SESE_ARCH_X64
#ifdef SESE_PLATFORM_WINDOWS
    // CASE: SESE_PLATFORM_WINDOWS && SESE_ARCH_X64
    return __rdtsc();
#else
    // CASE: !SESE_PLATFORM_WINDOWS && SESE_ARCH_X64
    uint64_t value;
    __asm__ __volatile__("rdtsc"
                         : "=A"(value));
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

} // namespace sese

sese::Random::Random([[maybe_unused]] const std::string &token) {
    this->_seed = Random::noise();
}

sese::Random::ResultType sese::Random::min() {
    return 0;
}

sese::Random::ResultType sese::Random::max() {
    return UINT64_MAX;
}

double sese::Random::entropy() const { // NOLINT
    return 64;
}

sese::Random::ResultType sese::Random::operator()() {
    auto unit = reinterpret_cast<LongLongSplitter *>(&_seed);
    _seed = (unit->low * MULTIPLIER + ADDEND) & MASK;
    (void) unit->high;
    return _seed ^ noise();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif