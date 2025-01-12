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

/// \file FileNotifier.h
/// \version 0.1
/// \author kaoru
/// \date March 11, 2023
/// \brief File Change Monitor

#pragma once

#include "sese/util/ErrorCode.h"
#include "sese/util/Result.h"

#include <functional>
#include <memory>

namespace sese::system {

/// \brief File Change Monitor
/// \bug This implementation has an inconsistent event order in Darwin compared to Windows and Linux. <p>
/// In Darwin, multiple events for the same file are triggered first, while in Windows and Linux, events are triggered in chronological order.
class FileNotifier final {
public:
    using Ptr = std::unique_ptr<FileNotifier>;
    using OnCreateCallback = std::function<void(std::string_view)>;
    using OnMoveCallback = std::function<void(std::string_view, std::string_view)>;
    using OnModifyCallback = std::function<void(std::string_view)>;
    using OnDeleteCallback = std::function<void(std::string_view)>;

    /// Create file monitor
    /// \param path The path to the directory
    /// \retval nullptr Creation failed
    static Ptr create(const std::string &path) noexcept;

    static Result<Ptr, ErrorCode> createEx(const std::string &path) noexcept;

    ~FileNotifier() noexcept;

    /// Start a thread to start processing the change event
    void start() const noexcept;

    /// Shut down the monitor and block until the background thread exits
    void shutdown() const noexcept;

    /// Set the callback function to be called when a file is created
    /// \param callback Callback function
    void setOnCreate(OnCreateCallback &&callback) const noexcept;

    /// Set the callback function to be called when a file is moved
    /// \param callback Callback function
    void setOnMove(OnMoveCallback &&callback) const noexcept;

    /// Set the callback function to be called when a file is modified
    /// \param callback Callback function
    void setOnModify(OnModifyCallback &&callback) const noexcept;

    /// Set the callback function to be called when a file is deleted
    /// \param callback Callback function
    void setOnDelete(OnDeleteCallback &&callback) const noexcept;

private:
    FileNotifier() = default;

    class Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace sese::system