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
 * @file Singleton.h
 * @brief Singleton Template Class
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once
#include "sese/Config.h"

namespace sese {

/**
 * @brief Raw Pointer Singleton Template Class
 * @tparam T
 * @tparam X
 * @tparam N
 */
template<typename T, class X = void, int N = 0>
class Singleton {
public:
    static T *getInstance() {
        static T v;
        return &v;
    }
};

/**
 * @brief Smart Pointer Singleton Template Class
 * @tparam T
 * @tparam X
 * @tparam N
 */
template<typename T, class X = void, int N = 0>
class SingletonPtr {
public:
    static std::shared_ptr<T> getInstance() {
        static std::shared_ptr<T> v(new T); // GCOVR_EXCL_LINE
        return v;
    }
};

} // namespace sese