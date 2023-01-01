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
#include <queue>
#include <mutex>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /// 对象池
    /// \tparam T 池中对象类型
    template<typename T>
    class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>>, public Noncopyable {
    public:
        /// 对象池智能指针
        using Ptr = std::shared_ptr<ObjectPool<T>>;
        /// 对象池对象智能指针
        using ObjectPtr = std::shared_ptr<T>;

        /// 在且仅能在堆上创建对象池
        /// \return 对象池智能指针
        static Ptr create() {
            return std::shared_ptr<ObjectPool<T>>(new ObjectPool<T>);
        }

        /// 从对象池中获取一个可复用对象
        /// \return 对象
        std::shared_ptr<T> borrow() {
            mutex.lock();
            std::shared_ptr<T> object;
            if (queue.empty()) {
                mutex.unlock();
                object = std::make_shared<T>();
                object.reset(object.get(), std::bind(&ObjectPool<T>::recycleCallback, std::weak_ptr<ObjectPool<T>>(this->shared_from_this()), object));
            } else {
                ObjectPtr p = queue.front();
                queue.pop();
                mutex.unlock();
                object.reset(p.get(), std::bind(&ObjectPool<T>::recycleCallback, std::weak_ptr<ObjectPool<T>>(this->shared_from_this()), p));
            }
            return object;
        }

    private:
        /// 私有构造函数
        ObjectPool() = default;

        /// 回收对象
        /// \param t
        void recycle(ObjectPtr t) {
            mutex.lock();
            queue.push(t);
            mutex.unlock();
        }

        /// 已借出对象的销毁器
        /// \param wkPool 对象所属对象池的弱指针，用于执行回收逻辑
        /// \param t 待回收对象
        static void recycleCallback(const std::weak_ptr<ObjectPool<T>> &wkPool, ObjectPtr t) {
            std::shared_ptr<ObjectPool<T>> pool(wkPool.lock());
            if (pool) {
                pool->recycle(t);
            } else {
                // t will be deleted
            }
        }

        std::mutex mutex;
        std::queue<ObjectPtr> queue;
    };
}// namespace sese