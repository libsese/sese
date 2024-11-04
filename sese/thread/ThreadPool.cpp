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

#include "sese/thread/ThreadPool.h"
#include "sese/thread/Locker.h"

#include <algorithm>

namespace sese {
ThreadPool::ThreadPool(std::string thread_pool_name, size_t threads)
    : name(std::move(thread_pool_name)),
      threads(std::max<size_t>(2, threads)),
      data(std::make_shared<RuntimeData>()) {

    auto proc = [data = data] {
        while (true) {
            std::unique_lock<std::mutex> locker(data->mutex);
            if (data->isShutdown) {
                locker.unlock();
                break;
            } else if (data->tasks.empty()) {
                data->conditionVariable.wait(locker);
            } else {
                auto task = std::move(data->tasks.front());
                data->tasks.pop();
                locker.unlock();
                if (task != nullptr) {
                    task();
                }
            }
        }
    };

    for (size_t i = 0; i < threads; i++) {
        auto pthread = new Thread(proc, name + std::to_string(i));
        threadGroup.emplace_back(pthread);
        pthread->start();
    }
}

ThreadPool::~ThreadPool() {
    if (!data->isShutdown) {
        shutdown();
    }
}

void ThreadPool::postTask(const std::function<void()> &task) {
    {
        Locker locker(data->mutex);
        data->tasks.emplace(task);
    }
    data->conditionVariable.notify_one();
}

void ThreadPool::postTask(const std::vector<std::function<void()>> &tasks) {
    {
        Locker locker(data->mutex);
        for (const auto &task: tasks) {
            data->tasks.emplace(task);
        }
    }
    data->conditionVariable.notify_all();
}

void ThreadPool::shutdown() {
    data->isShutdown = true;
    data->conditionVariable.notify_all();
    for (auto pthread: threadGroup) {
        // if (pthread->joinable()) {
        pthread->join();
        delete pthread; // GCOVR_EXCL_LINE
        // }
    }
}

size_t ThreadPool::size() noexcept {
    Locker locker(data->mutex);
    return data->tasks.size();
}

bool ThreadPool::empty() noexcept {
    Locker locker(data->mutex);
    return data->tasks.empty();
}
} // namespace sese