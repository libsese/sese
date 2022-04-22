/**
 * @file Singleton.h
 * @brief 单例模板类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "sese/Config.h"

namespace sese {

    /**
     * @brief 裸指针单例模板类
     * @tparam T
     * @tparam X
     * @tparam N
     */
    template<typename T, class X = void, int N = 0>
    class API Singleton {
    public:
        static T *getInstance() {
            static T v;
            return &v;
        }
    };

    /**
     * @brief 智能指针单例模板类
     * @tparam T
     * @tparam X
     * @tparam N
     */
    template<typename T, class X = void, int N = 0>
    class API SingletonPtr {
    public:
        static std::shared_ptr<T> getInstance() {
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };

}// namespace sese