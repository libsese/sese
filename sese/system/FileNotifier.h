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

#include <atomic>
#ifdef __APPLE__
#include <tuple>
#else
#include "sese/thread/Thread.h"
#endif

namespace sese::system {

/// File Change Callback Option Class
struct FileNotifyOption {
    virtual ~FileNotifyOption() = default;

    /// Create file
    virtual void onCreate(std::string_view name) = 0;

    /// Move file
    virtual void onMove(std::string_view src_name, std::string_view dst_name) = 0;

    /// Modify file
    virtual void onModify(std::string_view name) = 0;

    /// Delete file
    virtual void onDelete(std::string_view name) = 0;
};

/// \brief File Change Monitor
/// \bug This implementation has an inconsistent event order in Darwin compared to Windows and Linux. <p>
/// In Darwin, multiple events for the same file are triggered first, while in Windows and Linux, events are triggered in chronological order.
class FileNotifier {
public:
    using Ptr = std::unique_ptr<FileNotifier>;

    /// Create file monitor
    /// \param path The path to the [directory]
    /// \param option Callback options
    /// \retval nullptr Creation failed
    static FileNotifier::Ptr create(const std::string &path, FileNotifyOption *option) noexcept;

    static Result<Ptr, ErrorCode> createEx(const std::string &path, FileNotifyOption *option) noexcept;

    virtual ~FileNotifier() noexcept;

    /// Start a thread to start processing the change event
    void loopNonblocking() noexcept;

    /// Shut down the monitor and block until the background thread exits
    void shutdown() noexcept;

private:
    FileNotifier() = default;

#ifdef WIN32
    void *file_handle = nullptr;
    void *overlapped = nullptr;
    std::atomic_bool is_shutdown = false;
    FileNotifyOption *option = nullptr;
    Thread::Ptr th = nullptr;
#elif __linux__
    int inotify_fd = -1;
    int watch_fd = -1;
    std::atomic_bool is_shutdown = false;
    FileNotifyOption *option = nullptr;
    Thread::Ptr th = nullptr;
#elif __APPLE__
    void *stream = nullptr;
    void *queue = nullptr;
#endif
};
} // namespace sese::system