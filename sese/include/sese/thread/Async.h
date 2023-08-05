#pragma once

#include <sese/thread/ThreadPool.h>

namespace sese {

    template<class ReturnType>
    std::packaged_task<ReturnType()> async(const std::function<ReturnType()> &task) noexcept;

    template<class ReturnType>
    std::packaged_task<ReturnType()> async(ThreadPool &pool, const std::function<ReturnType()> &task) noexcept;

}// namespace sese

template<class ReturnType>
std::packaged_task<ReturnType()> sese::async(const std::function<ReturnType()> &task) noexcept {
    std::packaged_task<ReturnType()> packagedTask(task);

    auto th = std::thread([&](){ packagedTask(); });
    th.detach();

    // auto th = sese::Thread([&](){ packagedTask(); }, "Async");
    // th.start();
    // th.detach();

    return packagedTask;
}

template<class ReturnType>
std::packaged_task<ReturnType()> sese::async(ThreadPool &pool, const std::function<ReturnType()> &task) noexcept {
    return pool.postTask<ReturnType>(task);
}