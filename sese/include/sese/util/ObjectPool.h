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

        ~ObjectPool() {
            while(!queue.empty()) {
                delete queue.front();
                queue.pop();
            }
        }

        static ObjectPool::Ptr create() noexcept {
            return std::shared_ptr<ObjectPool<T>>(new ObjectPool<T>);
        }

        auto borrow() noexcept {
            mutex.lock();
            if (queue.empty()) {
                mutex.unlock();
                auto t = new T;
                return std::shared_ptr<T>(
                        t,
                        [wkPool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wkPool, t); }
                );
            } else {
                auto t = queue.front();
                queue.pop();
                mutex.unlock();
                return std::shared_ptr<T>(
                        t,
                        [wkPool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wkPool, t); }
                );
            }
        }

    private:
        ObjectPool() = default;

        static void recycleCallback(const std::weak_ptr<ObjectPool> &wkPool, T *toRecycle) noexcept {
            auto pool = wkPool.lock();
            if (pool) {
                // to recycle
                pool->mutex.lock();
                pool->queue.emplace(toRecycle);
                pool->mutex.unlock();
            } else {
                // to delete
                delete toRecycle;
            }
        }

        std::mutex mutex;
        std::queue<T *> queue;
    };
}// namespace sese