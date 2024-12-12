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
 * @file Uuid.h
 * @brief Uuid Class
 * @author kaoru
 * @version 0.1
 * @date September 25, 2023
 */

#pragma once

#include <cstdint>

namespace sese {

/// Uuid Class
class Uuid {
public:
    Uuid() noexcept = default;
    /// Create UUID
    /// \param self_id Device ID
    /// \param timestamp Timestamp
    /// \param r Reserved bit
    Uuid(uint8_t self_id, uint64_t timestamp, uint8_t r = 0) noexcept;

    /// Get Device ID
    /// \return Device ID
    [[nodiscard]] uint8_t getSelfId() const noexcept;
    /// Set Device ID
    /// \param i Device ID
    void setSelfId(uint8_t i) noexcept;
    /// Get Reserved bit
    /// \return Reserved bit
    [[nodiscard]] uint8_t getR() const noexcept;
    /// Set Reserved bit
    /// \param r Reserved bit
    void setR(uint8_t r) noexcept;
    /// Get Timestamp
    /// \return Timestamp
    [[nodiscard]] uint64_t getTimestamp() const noexcept;
    /// Set Timestamp
    /// \param i Timestamp
    void setTimestamp(uint64_t i) noexcept;

    /// Output numeric format UUID
    /// \return Numeric format UUID
    [[nodiscard]] uint64_t toNumber() const noexcept;
    /// Parse numeric format UUID to Uuid object
    /// \param number Numeric format UUID
    void parse(uint64_t number) noexcept;

protected:
    uint8_t selfId = 0;
    uint8_t r = 0;
    uint64_t timestamp = 0;
};

} // namespace sese
