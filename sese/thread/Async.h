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

/// \file Async.h
/// \brief Asynchronous task executor
/// \date August 5, 2023
/// \author kaoru

#pragma once

#include <sese/thread/GlobalThreadPool.h>

#include <coroutine>

namespace sese {

/// \brief Launch an anonymous thread to execute a task
/// \tparam RETURN_TYPE Return type
/// \param task Task
/// \return std::shared_future object
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> async(const std::function<RETURN_TYPE()> &task) noexcept;

/// \brief Submit a task to an existing thread pool
/// \tparam RETURN_TYPE Return type
/// \param pool Existing thread pool
/// \param task Task
/// \return std::shared_future object
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> async(ThreadPool &pool, const std::function<RETURN_TYPE()> &task) noexcept;

/// \brief Submit a task to the global thread pool
/// \tparam RETURN_TYPE Return type
/// \param task Task
/// \return std::shared_future object
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> asyncWithGlobalPool(const std::function<RETURN_TYPE()> &task) noexcept;

template<class T>
class FutureAwaiter {
public:
    explicit FutureAwaiter(std::shared_future<T> future) : future(future) {}

    [[nodiscard]] bool await_ready() noexcept {
        return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        future.wait();
        handle.resume();
    }

    T await_resume() noexcept {
        return future.get();
    }

private:
    std::shared_future<T> future;
};

} // namespace sese

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::async(const std::function<RETURN_TYPE()> &task) noexcept {
    std::packaged_task<RETURN_TYPE()> packaged_task(task);
    std::shared_future<RETURN_TYPE> future(packaged_task.get_future());

    std::thread(
        [&](std::packaged_task<RETURN_TYPE()> task) {
            task();
        },
        std::move(packaged_task)
    )
        .detach();

    return future;
}

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::async(ThreadPool &pool, const std::function<RETURN_TYPE()> &task) noexcept {
    return pool.postTask<RETURN_TYPE>(task);
}

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::asyncWithGlobalPool(const std::function<RETURN_TYPE()> &task) noexcept {
    return sese::GlobalThreadPool::postTask<RETURN_TYPE>(task);
}
