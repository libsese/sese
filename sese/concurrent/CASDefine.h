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
 * @file CASDefine.h
 * @brief 包含并发数据结构所用 API 和 类型
 * @author kaoru
 * @date 2022年5月31日
 */
#pragma once
#include <sese/Config.h>

namespace sese::concurrent {
/// \brief 节点
/// \tparam T 数据类型
template<typename T>
struct Node {
    T value{};
    Node *next{nullptr};
};
} // namespace sese::concurrent

/**
 * @brief 比较并交换两个指针
 * @verbatim
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
extern "C" inline bool compareAndSwapPointer(void *volatile *object, void *oldValue, void *newValue) noexcept;

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