/**
 * @file Random.h
 * @brief 基于线性同余发生器的随机类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Config.h"

namespace sese {
    /**
     * @brief 基于线性同余发生器的随机类
     */
    class API Random {
    private:
        static const uint64_t multiplier;
        static const uint64_t addend;
        static const uint64_t mask;
        static uint64_t seed;

    public:
        static uint64_t next();
    };
}// namespace sese