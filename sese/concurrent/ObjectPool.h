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
* @author Kaoru
* @date January 12, 2023
* @brief Concurrent object pool
* @version 0.2
*/

#pragma once

#include <sese/Config.h>
#include <sese/concurrent/LinkedQueue.h>
#include <sese/util/Noncopyable.h>
#include <functional>

namespace sese::concurrent {

// GCOVR_EXCL_START

/// \brief Concurrent object pool
/// \tparam T The type of objects in the pool
template<typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>>, public Noncopyable {
public:
    using Ptr = std::shared_ptr<ObjectPool<T>>;
    using ObjectPtr = std::shared_ptr<T>;

    ~ObjectPool() override {
        while (!queue.empty()) {
            T *p = nullptr;
            if (queue.pop(p)) {
                delete p;
            }
        }
    }

    /// The object pool can only be created on the heap
    /// \return A smart pointer to the object pool
    static Ptr create() {
        return std::shared_ptr<ObjectPool<T>>(new ObjectPool<T>);
    }

    /// Retrieves a reusable object from the object pool
    /// \return The object
    ObjectPtr borrow() {
        T *p = nullptr;
        if (queue.pop(p)) {
            return std::shared_ptr<T>(
                    p,
                    [wk_pool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wk_pool, t); }
            );
        } else {
            p = new T;
            return std::shared_ptr<T>(
                    p,
                    [wk_pool = this->weak_from_this()](T *t) { ObjectPool<T>::recycleCallback(wk_pool, t); }
            );
        }
    }

private:
    ObjectPool() = default;

    /// Destructor for the borrowed object
    /// \param wk_pool A weak pointer to the object pool that the object belongs to, used for triggering the recycling logic
    /// \param t The object to be recycled
    static void recycleCallback(const std::weak_ptr<ObjectPool<T>> &wk_pool, T *t) {
        if (auto pool = wk_pool.lock()) {
            pool->queue.push(t);
        } else {
            delete t;
        }
    }

    LinkedQueue<T *> queue;
};

// GCOVR_EXCL_STOP

} // namespace sese::concurrent