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
 * @file FileLocker.h
 * @brief kaoru
 * @author 文件锁
 */

#pragma once

#include <sese/Config.h>

namespace sese::system {

/**
 * @brief 文件锁
 */
class  FileLocker {
public:
    /// 根据文件描述符初始化一个文件锁
    /// \param fd 文件描述符
    explicit FileLocker(int32_t fd) : fd(fd) {}

    /// 为当前文件上写锁
    /// \param start 起始偏移
    /// \param len 长度
    /// \return 是否加锁成功
    [[nodiscard]] bool lockWrite(int64_t start, int64_t len);

    /// 为当前文件上读锁
    /// \param start 起始偏移
    /// \param len 长度
    /// \return 是否加锁成功
    [[nodiscard]] bool lockRead(int64_t start, int64_t len);

    /// 为当前文件上锁
    /// \param start 起始偏移
    /// \param len 长度
    /// \return 是否加锁成功
    [[nodiscard]] bool lock(int64_t start, int64_t len);

    /// 为当前文件解锁
    /// \return 是否解锁成功
    [[nodiscard]] bool unlock();

private:
    int32_t fd{};
#ifdef SESE_PLATFORM_WINDOWS
    int64_t offset{};
    int64_t size{};
#endif
};

} // namespace sese::system