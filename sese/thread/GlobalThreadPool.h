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

/// \file GlobalThreadPool.h
/// \author kaoru
/// \brief Global thread pool
/// \date August 6, 2023

#pragma once

#include <sese/Config.h>
#include <sese/thread/ThreadPool.h>
#include <sese/util/Singleton.h>

namespace sese {

/// Global thread pools, which use system thread pools on Windows, and slacker singleton thread pools on UNIX platforms
class GlobalThreadPool {
public:
    static void postTask(const std::function<void()> &func);

    template<class RETURN_TYPE>
    static std::shared_future<RETURN_TYPE> postTask(const std::function<RETURN_TYPE()> &func);

#ifdef SESE_PLATFORM_WINDOWS
private:
    struct Task1 {
        std::function<void()> function;
    };

    template<class RETURN_TYPE>
    struct Task2 {
        std::packaged_task<RETURN_TYPE()> packagedTask;
    };

    static DWORD WINAPI taskRunner1(LPVOID lp_param);

    template<class RETURN_TYPE>
    static DWORD WINAPI taskRunner2(LPVOID lp_parma);
#else
private:
    static SingletonPtr<ThreadPool> globalThreadPool;
#endif
};
} // namespace sese

#ifdef SESE_PLATFORM_WINDOWS

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::GlobalThreadPool::postTask(const std::function<RETURN_TYPE()> &func) {
    auto task2 = new Task2<RETURN_TYPE>();
    auto packaged_task = std::packaged_task<RETURN_TYPE()>(func);
    std::shared_future<RETURN_TYPE> future(packaged_task.get_future());
    task2->packagedTask = std::move(packaged_task);
    QueueUserWorkItem(taskRunner2<RETURN_TYPE>, task2, WT_EXECUTEDEFAULT);
    return future;
}

template<class RETURN_TYPE>
DWORD sese::GlobalThreadPool::taskRunner2(LPVOID lp_parma) {
    auto task2 = static_cast<Task2<RETURN_TYPE> *>(lp_parma);
    task2->packagedTask();
    delete task2;
    return 0;
}

#else

template<class ReturnType>
std::shared_future<ReturnType> sese::GlobalThreadPool::postTask(const std::function<ReturnType()> &func) {
    auto pool = globalThreadPool.getInstance();
    return pool->postTask<ReturnType>(func);
}

#endif