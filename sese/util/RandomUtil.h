/**
 * @file RandomUtil.h
 * @brief 随机工具类
 * @author kaoru
 * @version 0.1
 * @date 2023年10月1日
 */

#pragma once

#include <cmath>
#include <random>

namespace sese {
/// 随机工具类
class RandomUtil {
public:
    static constexpr int REQUIRED_UPPER_LETTER = 1;
    static constexpr int REQUIRED_LOWER_LETTER = 2;
    static constexpr int REQUIRED_DIGIT = 4;
    static constexpr int REQUIRED_SYMBOL = 8;

    template<class RETURN_VALUE>
    static RETURN_VALUE next();

    template<class RETURN_VALUE>
    static RETURN_VALUE next(RETURN_VALUE min, RETURN_VALUE max);

    static std::string nextString(size_t length, int required);

    static uint64_t nextUnsignedInt64();

    static std::random_device &getDevInstance() noexcept;

private:
    static std::string nextString(size_t length, bool upper, bool lower, bool digit, bool symbol);

    static const std::string UPPER_LETTER_LIST;
    static const std::string LOWER_LETTER_LIST;
    static const std::string DIGIT_LIST;
    static const std::string SYMBOL_LIST;

    static std::random_device dev;
};
} // namespace sese

template<class RETURN_VALUE>
inline RETURN_VALUE sese::RandomUtil::next() {
    return static_cast<RETURN_VALUE>(getDevInstance()());
}

template<class RETURN_VALUE>
inline RETURN_VALUE sese::RandomUtil::next(RETURN_VALUE min, RETURN_VALUE max) {
    const auto TEMP_MIN = std::min<RETURN_VALUE>(min, max);
    const auto TEMP_MAX = std::max<RETURN_VALUE>(min, max);
    const auto RANGE = TEMP_MAX - TEMP_MIN;
    auto TMP = getDevInstance()();
    return static_cast<RETURN_VALUE>(TMP % RANGE) + min;
}

template<>
inline float sese::RandomUtil::next() {
    auto pos = next<uint32_t>(0, UINT32_MAX);
    return static_cast<float>(pos) / static_cast<float>(UINT32_MAX);
}

template<>
inline double sese::RandomUtil::next() {
    auto pos = next<uint32_t>(0, UINT32_MAX);
    return static_cast<double>(pos) / static_cast<double>(UINT32_MAX);
}

template<>
inline float sese::RandomUtil::next(float min, float max) {
    const auto TMP_MIN = std::min<float>(min, max);
    const auto RANGE = std::abs(max - min);
    const auto TMP = next<float>();
    return TMP_MIN + RANGE * TMP;
}

template<>
inline double sese::RandomUtil::next(double min, double max) {
    const auto TMP_MIN = std::min<double>(min, max);
    const auto RANGE = std::abs(max - min);
    const auto TMP = next<double>();
    return TMP_MIN + RANGE * TMP;
}