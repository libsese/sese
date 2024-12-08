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
 * @file Locker.h
 * @author kaoru
 * @brief Custom std::mutex wrapper
 * @date April 5, 2022
 */

#pragma once
#include "sese/Config.h"
#include <memory>
#include <mutex>

namespace sese {

/**
 * @brief std::mutex wrapper
 * @tparam T std::mutex
 */
template<typename T = std::mutex>
class  Locker {
public:
    explicit Locker(T &t) : mutex(t) {
        mutex.lock();
    }
    ~Locker() {
        mutex.unlock();
    }

private:
    T &mutex;
};

} // namespace sese
