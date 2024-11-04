// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file ObjectPool.h
 * @author kaoru
 * @date 2022年7月8日
 * @brief 对象池
 * @version 0.2
 */

#pragma once

#include "Noncopyable.h"
#include "sese/Config.h"

#include <functional>
#include <memory>
#include <queue>
#include <mutex>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

/// \brief 对象池
/// \tparam T 复用对象
template<typename T>
class ObjectPool
    : public std::enable_shared_from_this<ObjectPool<T>>,
      public Noncopyable {
public:
    using Ptr = std::shared_ptr<ObjectPool<T>>;
    using ObjectPtr = std::shared_ptr<T>;

    ~ObjectPool() override {
        while (!queue.empty()) {
            delete queue.front();
            queue.pop();
        }
    }

    static ObjectPool::Ptr create() noexcept {
        return std::make_shared_for_overwrite<ObjectPool<T>> ();
    }

    auto borrow() noexcept {
        mutex.lock();
        if (queue.empty()) {
            mutex.unlock();
            auto t = new T;
            return std::shared_ptr<T>(
                    t,
                    [wk_pool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wk_pool, t); }
            );
        } else {
            auto t = queue.front();
            queue.pop();
            mutex.unlock();
            return std::shared_ptr<T>(
                    t,
                    [wk_pool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wk_pool, t); }
            );
        }
    }

private:
    ObjectPool() = default;

    static void recycleCallback(const std::weak_ptr<ObjectPool> &wk_pool, T *to_recycle) noexcept {
        auto pool = wk_pool.lock();
        if (pool) {
            // to recycle
            pool->mutex.lock();
            pool->queue.emplace(to_recycle);
            pool->mutex.unlock();
        } else {
            // to delete
            delete to_recycle;
        }
    }

    std::mutex mutex;
    std::queue<T *> queue;
};
} // namespace sese