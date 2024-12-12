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
 * @file Thread.h
 * @brief Thread class
 * @author kaoru
 * @date March 28, 2022
 * @bug Improper handling of thread detachment functionality may lead to memory leaks. For detached threads, it is recommended to consider using standard library threads.
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/Initializer.h"
#include "sese/util/Noncopyable.h"

#include <functional>
#include <thread>

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable : 4251)
#pragma warning(disable : 4624)
#endif

namespace sese {

/// \brief Initialization task for the built-in thread class
class ThreadInitiateTask final : public InitiateTask {
public:
    ThreadInitiateTask() : InitiateTask(__FUNCTION__) {};

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/**
 * @brief Thread class
 */
class Thread final : public Noncopyable { // GCOVR_EXCL_LINE
public:
    using Ptr = std::unique_ptr<Thread>;

    struct RuntimeData;

    explicit Thread(const std::function<void()> &function, const std::string &name = THREAD_DEFAULT_NAME);
    Thread(Thread &thread);
    ~Thread() override;

    void start() const;
    void join() const;
    void detach() const;
    [[nodiscard]] bool joinable() const;

    static void run(std::shared_ptr<RuntimeData> data);

    [[nodiscard]] tid_t getTid() const noexcept { return data->id; }
    [[nodiscard]] const std::string &getThreadName() const noexcept { return this->data->name; }

public:
    /// Runtime data of threads
    struct RuntimeData {
        tid_t id = 0;

        std::string name;
        std::thread th;
        std::function<void()> function;
    };

    static tid_t getCurrentThreadId() noexcept;
    static const char *getCurrentThreadName() noexcept;
    static void setCurrentThreadAsMain() noexcept;
    static tid_t getMainThreadId() noexcept;

    /**
     * \brief Get the current thread instance
     * \return The current thread instance, returns nullptr if the current thread is the main thread
     */
    static Thread::RuntimeData *getCurrentThreadData() noexcept;

private:
    static tid_t main_id;
    std::shared_ptr<RuntimeData> data = nullptr;
};

} // namespace sese