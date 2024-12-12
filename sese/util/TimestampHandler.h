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
 * @file TimestampHandler.h
 * @brief Timestamp Handler
 * @author kaoru
 * @version 0.1
 * @date September 25, 2023
 */

#pragma once

#include <chrono>
#include <cstdint>

namespace sese {

/// Timestamp Handler
class TimestampHandler {
public:
    explicit TimestampHandler(std::chrono::system_clock::time_point point) noexcept;
    /// Get the current timestamp
    /// \return Millisecond-level timestamp
    uint64_t getCurrentTimestamp() noexcept;
    /// Try to get the current timestamp
    /// \retval 0 System clock rollback, and rollback time is less than or equal to 5 seconds
    /// \retval UINT64_MAX System clock rollback, and rollback time is greater than 5 seconds
    uint64_t tryGetCurrentTimestamp() noexcept;

protected:
    std::chrono::milliseconds latest{};
};

} // namespace sese
