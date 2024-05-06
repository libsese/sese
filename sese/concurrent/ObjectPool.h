/**
* @file ObjectPool.h
* @author kaoru
* @date 2023年1月12日
* @brief 并发对象池
* @version 0.2
*/

#pragma once

#include <sese/Config.h>
#include <sese/concurrent/LinkedQueue.h>
#include <sese/util/Noncopyable.h>
#include <functional>

namespace sese::concurrent {

// GCOVR_EXCL_START

/// \brief 并发对象池
/// \tparam T 池中对象类型
template<typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>>, public Noncopyable {
public:
    /// 对象池智能指针
    using Ptr = std::shared_ptr<ObjectPool<T>>;
    /// 对象池对象智能指针
    using ObjectPtr = std::shared_ptr<T>;

    ~ObjectPool() override {
        while (!queue.empty()) {
            T *p = nullptr;
            if (queue.pop(p)) {
                delete p;
            }
        }
    }

    /// 在且仅能在堆上创建对象池
    /// \return 对象池智能指针
    static Ptr create() {
        return std::shared_ptr<ObjectPool<T>>(new ObjectPool<T>);
    }

    /// 从对象池中获取一个可复用对象
    /// \return 对象
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
    /// 私有构造函数
    ObjectPool() = default;

    /// 已借出对象的销毁器
    /// \param wk_pool 对象所属对象池的弱指针，用于执行回收逻辑
    /// \param t 待回收对象
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