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

/// \file Process.h
/// \brief Process Class
/// \author kaoru
/// \version 0.1
/// \date March 13, 2023

#pragma once

#include "sese/Util.h"
#include "sese/util/ErrorCode.h"
#include "sese/util/Result.h"

#include <vector>

namespace sese::system {

/// Process Class
class Process {
public:
    using Ptr = std::unique_ptr<Process>;

    static Result<Process::Ptr, ErrorCode> createEx(const std::string &exec, std::vector<std::string> args = {}) noexcept;

    /// Get the current process ID
    /// \return Current process ID
    [[nodiscard]] static pid_t getCurrentProcessId() noexcept;

    /// Wait for the process to end
    /// \return Process exit code
    [[nodiscard]] int wait() const noexcept;

    /// Kill the process
    /// \return Operation result
    [[nodiscard]] bool kill() const noexcept;

    /// Get the process ID
    /// \return Process object ID
    [[nodiscard]] pid_t getProcessId() const noexcept;

private:
    Process() = default;

#ifdef _WIN32
public:
    virtual ~Process() noexcept;

private:
    void *startup_info = nullptr;
    void *process_info = nullptr;
#else
public:
    virtual ~Process() noexcept = default;

private:
    pid_t id = -1;
#endif
};
} // namespace sese::system
