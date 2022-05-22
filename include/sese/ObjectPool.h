#pragma once
#include <sese/thread/Locker.h>
#include <sese/Config.h>
#include <functional>
#include <queue>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    template<typename T>
    class API ObjectPool {
    public:
        using Type = std::shared_ptr<T>;

        virtual ~ObjectPool() = default;

        Type borrowObject();
        void returnObject(const Type &object);

    protected:
        std::mutex mutex;
        std::queue<Type> objectPool;
    };
}// namespace sese

template<typename T>
typename sese::ObjectPool<T>::Type sese::ObjectPool<T>::borrowObject() {
    mutex.lock();
    if(objectPool.empty()) {
        mutex.unlock();
        return std::make_shared<T>();
    } else {
        auto object = objectPool.front();
        objectPool.pop();
        mutex.unlock();
        return object;
    }
}

template<typename T>
void sese::ObjectPool<T>::returnObject(const Type &object) {
    Locker locker(mutex);
    objectPool.emplace(object);
}