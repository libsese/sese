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
 * @file Random.h
 * @brief Random class based on linear congruential generator
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/Noncopyable.h"

namespace sese {
/**
 * @brief Random class based on linear congruential generator
 */
class SESE_DEPRECATED_WITH("Unsafe") Random : public Noncopyable {
    // for std::random_device
public:
    using ResultType = uint64_t;

    explicit Random(const std::string &token = "");

    // properties
    static ResultType min();
    static ResultType max();
    [[nodiscard]] double entropy() const;

    // generate
    ResultType operator()();

private:
    ResultType _seed;

    static const ResultType MULTIPLIER;
    static const ResultType ADDEND;
    static const ResultType MASK;
    static ResultType noise() noexcept;
};
} // namespace sese