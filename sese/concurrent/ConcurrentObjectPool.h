/**
* @file ConcurrentObjectPool.h
* @author kaoru
* @date 2023年1月12日
* @brief 并发对象池
* @version 0.2
*/
#pragma once
#include "sese/util/Noncopyable.h"

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

        ~ConcurrentObjectPool() {
            while (!queue.empty()) {
                auto p = queue.pop(nullptr);
                // assert p != nullptr
                delete p;
            }
        }

        /// 在且仅能在堆上创建对象池
        /// \return 对象池智能指针
        static Ptr create() {
            return std::shared_ptr<ConcurrentObjectPool<T>>(new ConcurrentObjectPool<T>);
        }

        /// 从对象池中获取一个可复用对象
        /// \return 对象
        ObjectPtr borrow() {
            T *p = queue.pop(nullptr);
            if (p != nullptr) {
                return std::shared_ptr<T>(
                        p,
                        [wkPool = this->weak_from_this()](T *t) {ConcurrentObjectPool<T>::recycleCallback(wkPool, t); }
                );
            } else {
                p = new T;
                return std::shared_ptr<T>(
                        p,
                        [wkPool = this->weak_from_this()](T *t) {ConcurrentObjectPool<T>::recycleCallback(wkPool, t); }
                );
            }
        }

    private:
        /// 私有构造函数
        ConcurrentObjectPool() = default;

        /// 已借出对象的销毁器
        /// \param wkPool 对象所属对象池的弱指针，用于执行回收逻辑
        /// \param t 待回收对象
        static void recycleCallback(const std::weak_ptr<ConcurrentObjectPool<T>> &wkPool, T *t) {
            auto pool = wkPool.lock();
            if (pool) {
                pool->queue.push(t);
            } else {
                delete t;
            }
        }

        LinkedQueue<T *> queue;
    };
}// namespace sese::concurrent