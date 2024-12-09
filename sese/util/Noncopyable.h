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
 * @file Noncopyable.h
 * @author kaoru
 * @date March 28, 2022
 * @brief Non-copyable class
 */


#pragma once

// GCOVR_EXCL_START

namespace sese {

/**
 * @brief Non-copyable class
 */
class Noncopyable {
public:
    Noncopyable() = default;
    virtual ~Noncopyable() = default;


    Noncopyable(const Noncopyable &) = delete;

    Noncopyable &operator=(const Noncopyable &) = delete;
};
} // namespace sese

// GCOVR_EXCL_STOP