/**
* @file ConcurrentObjectPool.h
* @author kaoru
* @date 2022年7月9日
* @brief 并发对象池
* @version 0.1
*/
#pragma once
#include <sese/Noncopyable.h>
#include <sese/Config.h>
#include <sese/concurrent/LinkedQueue.h>
#include <functional>

namespace sese::concurrent {

    /// 并发对象池
    /// \tparam T 池中对象类型
    template<typename T>
    class ConcurrentObjectPool : public std::enable_shared_from_this<ConcurrentObjectPool<T>>, public Noncopyable {
    public:
        /// 对象池智能指针
        using Ptr = std::shared_ptr<ConcurrentObjectPool<T>>;
        /// 对象池对象智能指针
        using ObjectPtr = std::shared_ptr<T>;

        /// 在且仅能在堆上创建对象池
        /// \return 对象池智能指针
        static Ptr create() {
            return std::shared_ptr<ConcurrentObjectPool<T>>(new ConcurrentObjectPool<T>);
        }

        /// 从对象池中获取一个可复用对象
        /// \return 对象
        ObjectPtr borrow() {
            ObjectPtr object;

            ObjectPtr p = queue.pop(ObjectPtr(nullptr));
            if (!p) {
                object = std::make_shared<T>();
                object.reset(object.get(), std::bind(&ConcurrentObjectPool<T>::recycleCallback, std::weak_ptr<ConcurrentObjectPool<T>>(this->shared_from_this()), object));
            } else {
                object = p;
                object.reset(p.get(), std::bind(&ConcurrentObjectPool<T>::recycleCallback, std::weak_ptr<ConcurrentObjectPool<T>>(this->shared_from_this()), p));
            }
            return object;
        }

    private:
        /// 私有构造函数
        ConcurrentObjectPool() = default;

        /// 回收对象
        /// \param t
        void recycle(ObjectPtr t) {
            queue.push(t);
        }

        /// 已借出对象的销毁器
        /// \param wkPool 对象所属对象池的弱指针，用于执行回收逻辑
        /// \param t 待回收对象
        static void recycleCallback(const std::weak_ptr<ConcurrentObjectPool<T>> &wkPool, ObjectPtr t) {
            std::shared_ptr<ConcurrentObjectPool<T>> pool(wkPool.lock());
            if (pool) {
                pool->recycle(t);
            } else {
                // t will be deleted
            }
        }

        LinkedQueue<ObjectPtr> queue;
    };
}// namespace sese::concurrent