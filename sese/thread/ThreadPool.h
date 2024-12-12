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
 * @file ThreadPool.h
 * @brief Thread pool class
 * @date April 7, 2022
 * @author kaoru
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/Noncopyable.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace sese {

/**
 * @brief Thread pool class
 */
class  ThreadPool : public Noncopyable {
public:
    using Ptr = std::unique_ptr<ThreadPool>;

    /**
     * \brief Initialize the thread pool
     * \param thread_pool_name Name of the thread pool (affects the names of the threads in the pool)
     * \param threads Number of threads
     */
    explicit ThreadPool(std::string thread_pool_name = THREAD_DEFAULT_NAME, size_t threads = 4);
    ~ThreadPool() override;

public:
    /**
     * \brief Add a single task to the thread pool
     * \param task The task to be executed
     */
    void postTask(const std::function<void()> &task);

    /**
     * \brief Add multiple tasks to the thread pool
     * \param tasks Collection of tasks to be executed
     */
    void postTask(const std::vector<std::function<void()>> &tasks);

    /**
     * \brief Add a task to the thread pool and bind parameters
     * \tparam FUNCTION Function template
     * \tparam ARGS Argument template
     * \param f Function
     * \param args Arguments
     */
    template<typename FUNCTION, typename... ARGS>
    void postTaskEx(FUNCTION &&f, ARGS &&...args) {
        auto bound_function = [func = std::forward<FUNCTION>(f), args = std::make_tuple(std::forward<ARGS>(args)...)]() mutable {
            std::apply(std::move(func), std::move(args));
        };
        this->postTask(std::move(bound_function));
    }

    /**
     * \brief Add a task with a return value to the thread pool
     * \tparam RETURN_TYPE Return type
     * \param tasks Tasks to be executed
     * \return std::shared_future object
     */
    template<class RETURN_TYPE>
    std::shared_future<RETURN_TYPE> postTask(const std::function<RETURN_TYPE()> &tasks);

    /**
     * \brief Shutdown the current thread pool and block until all child threads exit
     */
    void shutdown();

    [[nodiscard]] size_t size() noexcept;
    [[nodiscard]] bool empty() noexcept;
    [[nodiscard]] const std::string &getName() const { return name; }
    [[nodiscard]] size_t getThreads() const { return threads; }

private:
    std::string name;
    size_t threads = 0;
    std::vector<Thread *> threadGroup;

    /// Runtime data of thread pool
    struct RuntimeData {
        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::queue<std::function<void()>> tasks;
        std::atomic<bool> isShutdown{false};
    };
    std::shared_ptr<RuntimeData> data;
};

} // namespace sese

// GCOVR_EXCL_START

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::ThreadPool::postTask(const std::function<RETURN_TYPE()> &task) {
    /**
     * \brief
     * @verbatim
     * Since std::packaged_task is a non-copyable object,
     * and std::function erases the type information of its arguments, making std::move ineffective on non-copyable objects,
     * the solution here is to use std::shared_ptr to wrap std::packaged_task.
     * @endverbatim
     */
    using TaskPtr = std::shared_ptr<std::packaged_task<RETURN_TYPE()>>;
    TaskPtr packaged_task = std::make_shared<std::packaged_task<RETURN_TYPE()>>(task);
    std::shared_future<RETURN_TYPE> future(packaged_task->get_future());

    this->postTask([task = std::move(packaged_task)]() mutable {
        (*task)();
    });

    return future;
}

// GCOVR_EXCL_STOP