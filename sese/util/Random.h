/**
 * @file Random.h
 * @brief 基于线性同余发生器的随机类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include "sese/Config.h"
#include "sese/util/Noncopyable.h"

namespace sese {
    /**
     * @brief 基于线性同余发生器的随机类
     */
    class API Random : public Noncopyable {
        // for std::random_device
    public:
        using ResultType = uint64_t;

        explicit Random(const std::string &token = "");

        // properties
        static ResultType min();
        static ResultType max();
        [[nodiscard]] double entropy() const;

        // generate
        ResultType operator()();

    private:
        ResultType _seed;

        static const ResultType multiplier;
        static const ResultType addend;
        static const ResultType mask;
        static ResultType noise() noexcept;
    };
}// namespace sese