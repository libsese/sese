/**
 * @file RandomUtil.h
 * @brief 随机工具类
 * @author kaoru
 * @version 0.1
 * @date 2023年10月1日
 */

#pragma once

#include <sese/util/Random.h>

#include <cmath>

namespace sese {
    /// 随机工具类
    class RandomUtil {
    public:
        template<class ReturnValue>
        static ReturnValue next();

        template<class ReturnValue>
        static ReturnValue next(ReturnValue min, ReturnValue max);

    private:
        static Random dev;
    };
}// namespace sese

template<class ReturnValue>
ReturnValue sese::RandomUtil::next() {
    return static_cast<ReturnValue>(dev());
}

template<class ReturnValue>
ReturnValue sese::RandomUtil::next(ReturnValue min, ReturnValue max) {
    auto _min = std::min<ReturnValue>(min, max);
    auto _max = std::max<ReturnValue>(min, max);
    auto range = _max - _min;
    auto tmp = dev();
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