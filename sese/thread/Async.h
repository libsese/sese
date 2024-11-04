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
/// \brief 异步任务执行器
/// \date 2023年8月5日
/// \author kaoru

#pragma once

#include <sese/thread/GlobalThreadPool.h>

namespace sese {

/// 启动一个匿名线程执行任务
/// \tparam RETURN_TYPE 返回值类型
/// \param task 任务
/// \return std::shared_future 对象
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> async(const std::function<RETURN_TYPE()> &task) noexcept;

/// 将任务提交到一个现有的线程池当中
/// \tparam RETURN_TYPE 返回值类型
/// \param pool 现有的线程池
/// \param task 任务
/// \return std::shared_future 对象
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> async(ThreadPool &pool, const std::function<RETURN_TYPE()> &task) noexcept;

/// 将任务提交到全局线程池当中
/// \tparam RETURN_TYPE 返回值类型
/// \param task 任务
/// \return std::shared_future 对象
template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> asyncWithGlobalPool(const std::function<RETURN_TYPE()> &task) noexcept;

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