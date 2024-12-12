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
 * @author File locker
 */

#pragma once

#include <sese/Config.h>

namespace sese::system {

/**
 * @brief File locker
 */
class FileLocker {
public:
    /// Initialize a file lock based on the file descriptor
    /// \param fd File descriptor
    explicit FileLocker(int32_t fd) : fd(fd) {}

    /// Apply a write lock to the current file
    /// \param start Starting offset
    /// \param len Length
    /// \return Whether the lock was successfully applied
    [[nodiscard]] bool lockWrite(int64_t start, int64_t len);

    /// Apply a read lock to the current file
    /// \param start Starting offset
    /// \param len Length
    /// \return Whether the lock was successfully applied
    [[nodiscard]] bool lockRead(int64_t start, int64_t len);

    /// Apply a lock to the current file
    /// \param start Starting offset
    /// \param len Length
    /// \return Whether the lock was successfully applied
    [[nodiscard]] bool lock(int64_t start, int64_t len);

    /// Unlock the current file
    /// \return Whether the unlock operation was successful
    [[nodiscard]] bool unlock();

private:
    int32_t fd{};
#ifdef SESE_PLATFORM_WINDOWS
    int64_t offset{};
    int64_t size{};
#endif
};

} // namespace sese::system