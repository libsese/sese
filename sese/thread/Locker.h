/**
 * @file Locker.h
 * @author kaoru
 * @brief 自定义的 std::mutex 包装器
 * @date 2022年4月5日
 */
#pragma once
#include "sese/Config.h"
#include <memory>
#include <mutex>

namespace sese {

/**
 * @brief std::mutex 包装器
 * @tparam T std::mutex
 */
template<typename T = std::mutex>
class API Locker {
public:
    explicit Locker(T &t) : mutex(t) {
        mutex.lock();
    }
    ~Locker() {
        mutex.unlock();
    }

private:
    T &mutex;
};

} // namespace sese
