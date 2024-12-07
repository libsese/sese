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
 * @brief Contains APIs and types for concurrent data structures
 * @author Kaoru
 * @date May 31, 2022
 */

#pragma once
#include <sese/Config.h>

namespace sese::concurrent {

/// \brief Node
/// \tparam T The data type
template<typename T>
struct Node {
    T value{};
    Node *next{nullptr};
};
} // namespace sese::concurrent

/**
 * @brief Compares and swaps two pointers
 * @verbatim
 * if( *object == oldValue ) {
 *   *object = newValue;
 *   return true;
 * } else {
 *   return false;
 * }
 * @endverbatim
 * @param object The object to be compared
 * @param oldValue The original value
 * @param newValue The value to be set
 * @return Whether the operation was successful
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