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
    auto range = std::abs(max - min);
    auto tmp = static_cast<ReturnValue>(dev());
    return (tmp % range) + _min;
}

template<>
float sese::RandomUtil::next() {
    return static_cast<float>(dev()) / static_cast<float>(sese::Random::max());
}

template<>
double sese::RandomUtil::next() {
    return static_cast<double>(dev()) / static_cast<double>(sese::Random::max());
}

template<>
float sese::RandomUtil::next(float min, float max) {
    auto _min = std::min<float>(min, max);
    auto range = std::abs(max - min);
    auto tmp = next<float>();
    return _min + range * tmp;
}

template<>
double sese::RandomUtil::next(double min, double max) {
    auto _min = std::min<double>(min, max);
    auto range = std::abs(max - min);
    auto tmp = next<double>();
    return _min + range * tmp;
}