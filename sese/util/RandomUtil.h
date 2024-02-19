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
    static const int REQUIRED_UPPER_LETTER = 1;
    static const int REQUIRED_LOWER_LETTER = 2;
    static const int REQUIRED_DIGIT = 4;
    static const int REQUIRED_SYMBOL = 8;

    template<class ReturnValue>
    static ReturnValue next();

    template<class ReturnValue>
    static ReturnValue next(ReturnValue min, ReturnValue max);

    static std::string nextString(size_t length, int required);

    static uint64_t nextUnsignedInt64();

    static std::random_device &getDevInstance() noexcept;

private:
    static std::string nextString(size_t length, bool upper, bool lower, bool digit, bool symbol);

    static std::string UPPER_LETTER_LIST;
    static std::string LOWER_LETTER_LIST;
    static std::string DIGIT_LIST;
    static std::string SYMBOL_LIST;

    static std::random_device dev;
};
} // namespace sese

template<class ReturnValue>
inline ReturnValue sese::RandomUtil::next() {
    return static_cast<ReturnValue>(getDevInstance()());
}

template<class ReturnValue>
inline ReturnValue sese::RandomUtil::next(ReturnValue min, ReturnValue max) {
    auto _min = std::min<ReturnValue>(min, max);
    auto _max = std::max<ReturnValue>(min, max);
    auto range = _max - _min;
    auto tmp = getDevInstance()();
    return static_cast<ReturnValue>(tmp % range) + _min;
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
    auto _min = std::min<float>(min, max);
    auto range = std::abs(max - min);
    auto tmp = next<float>();
    return _min + range * tmp;
}

template<>
inline double sese::RandomUtil::next(double min, double max) {
    auto _min = std::min<double>(min, max);
    auto range = std::abs(max - min);
    auto tmp = next<double>();
    return _min + range * tmp;
}