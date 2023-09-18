/**
 * @file CASDefine.h
 * @brief 包含并发数据结构所用 API 和 类型
 * @author kaoru
 * @date 2022年5月31日
 */
#pragma once
#include <sese/Config.h>

namespace sese::concurrent {

    template<typename T>
    struct Node {
        T value{};
        Node *next{nullptr};
    };
}// namespace sese::concurrent

/**
 * @verbatim
 * 比较并交换两个指针
 * if( *object == oldValue ) {
 *   *object = newValue;
 *   return true;
 * } else {
 *   return false;
 * }
 * @endverbatim
 * @param object 欲比较对象
 * @param oldValue 原先的值
 * @param newValue 欲设置的值
 * @return 是否设置成功
 */
extern "C" API inline bool compareAndSwapPointer(void *volatile *object, void *oldValue, void *newValue) noexcept;

#ifdef _MSC_VER
bool compareAndSwapPointer(void *volatile *object, void *oldValue, void *newValue) noexcept {
    InterlockedCompareExchangePointer(object, newValue, oldValue);
    return *object == newValue;
}
#else
bool compareAndSwapPointer(void *volatile *object, void *oldValue, void *newValue) noexcept {
    return __sync_bool_compare_and_swap(object, oldValue, newValue);
}
#endif