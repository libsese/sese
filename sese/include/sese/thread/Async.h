/// \file Async.h
/// \brief 异步任务执行器
/// \date 2023年8月5日
/// \author kaoru

#pragma once

#include <sese/thread/ThreadPool.h>

namespace sese {

    /// 启动一个匿名线程执行任务
    /// \tparam ReturnType 返回值类型
    /// \param task 任务
    /// \return std::packaged_task 对象
    template<class ReturnType>
    std::packaged_task<ReturnType()> async(const std::function<ReturnType()> &task) noexcept;

    /// 将任务提交到一个现有的线程池当中
    /// \tparam ReturnType 返回值类型
    /// \param pool 现有的线程池
    /// \param task 任务
    /// \return std::packaged_task 对象
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