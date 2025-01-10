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

/// \file AsyncLogger.h
/// \author kaoru
/// \date June 11, 2023
/// \brief Asynchronous logger
/// \version 0.1.0
/// \bug Using valgrind tests will definitely produce memory leaks here,
/// but using the -fsanitize=leak option for testing everything works fine

#pragma once

#include <sese/log/Logger.h>
#include <sese/thread/Thread.h>
#include <sese/io/FixedBuilder.h>

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace sese::log {

/// Asynchronous logger
class  AsyncLogger final : public Logger {
public:
    AsyncLogger();

    ~AsyncLogger() noexcept override;

    void log(const Event::Ptr &event) noexcept override;

    void dump(const void *buffer, size_t length) noexcept override;

    void loop() noexcept;

protected:
    io::FixedBuilder *currentBuffer;
    io::FixedBuilder *nextBuffer;
    // This queue holds buffers that are ready to be committed
    std::vector<io::FixedBuilder *> buffer2Ready;

    std::mutex mutex;
    std::condition_variable conditionVariable;
    std::atomic_bool isShutdown{};
    sese::Thread::Ptr thread;
};
} // namespace sese::log