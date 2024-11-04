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
 * @brief 时间戳处理器
 * @author kaoru
 * @version 0.1
 * @date 2023年9月25日
 */

#pragma once

#include <chrono>
#include <cstdint>

namespace sese {

/// 时间戳处理器
class TimestampHandler {
public:
    explicit TimestampHandler(std::chrono::system_clock::time_point point) noexcept;
    /// 获取当前时间戳
    /// \return 毫秒级时间戳
    uint64_t getCurrentTimestamp() noexcept;
    /// 尝试获取当前时间戳
    /// \retval 0 系统时钟回拨，且回拨时间小于等于 5 秒
    /// \retval UINT64_MAX 系统时钟回拨，且回拨时间大于 5 秒
    uint64_t tryGetCurrentTimestamp() noexcept;

protected:
    std::chrono::milliseconds latest{};
};

} // namespace sese