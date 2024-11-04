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
 * @brief Uuid 类
 * @author kaoru
 * @version 0.1
 * @date 2023年9月25日
 */

#pragma once

#include <cstdint>

namespace sese {

/// Uuid 类
class Uuid {
public:
    Uuid() noexcept = default;
    /// 创建 UUID
    /// \param self_id 本机 ID
    /// \param timestamp 时间戳
    /// \param r 保留位
    Uuid(uint8_t self_id, uint64_t timestamp, uint8_t r = 0) noexcept;

    /// 获取本机 ID
    /// \return 本机 ID
    [[nodiscard]] uint8_t getSelfId() const noexcept;
    /// 设置本机 ID
    /// \param i 本机 ID
    void setSelfId(uint8_t i) noexcept;
    /// 获取保留位
    /// \return 保留位
    [[nodiscard]] uint8_t getR() const noexcept;
    /// 设置保留位
    /// \param r 保留位
    void setR(uint8_t r) noexcept;
    /// 获取时间戳
    /// \return 时间戳
    [[nodiscard]] uint64_t getTimestamp() const noexcept;
    /// 设置时间戳
    /// \param i 时间戳
    void setTimestamp(uint64_t i) noexcept;

    /// 输出纯数字格式 UUID
    /// \return 纯数字格式 UUID
    [[nodiscard]] uint64_t toNumber() const noexcept;
    /// 从纯数字格式 UUID 中解析为 UUID 对象
    /// \param number 纯数字格式 UUID
    void parse(uint64_t number) noexcept;

protected:
    uint8_t selfId = 0;
    uint8_t r = 0;
    uint64_t timestamp = 0;
};

} // namespace sese