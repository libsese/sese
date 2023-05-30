/**
 * @file Random.h
 * @brief 基于线性同余发生器的随机类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include "sese/Config.h"
#include "sese/util/Noncopyable.h"

#ifdef WIN32
#undef max
#undef min
#endif

namespace sese {
    /**
     * @brief 基于线性同余发生器的随机类
     */
    class API Random : public Noncopyable {
    private:
        static const uint64_t multiplier;
        static const uint64_t addend;
        static const uint64_t mask;
        static uint64_t seed;

    public:
        static uint64_t next() noexcept;

    private:
        static uint64_t noise() noexcept;

        // for std::random_device
    public:
        explicit Random(const std::string &token = "");

        // properties
        static uint64_t min();
        static uint64_t max();
        [[nodiscard]] double entropy() const;

        // generate
        uint64_t operator()();

    private:
        uint64_t _seed;
    };
}// namespace sese