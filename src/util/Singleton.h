#pragma once
#include <memory>

namespace sese {

    template<typename T>
    class Singleton {
    public:
        static T *getInstance() {
            static T v;
            return &v;
        }
    };

    template<typename T>
    class SingletonPtr {
    public:
        static std::shared_ptr<T> getInstance() {
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };

}// namespace sese