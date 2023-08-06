/// \file Async.h
/// \brief 异步任务执行器
/// \date 2023年8月5日
/// \author kaoru

#pragma once

#include <sese/thread/GlobalThreadPool.h>

namespace sese {

    /// 启动一个匿名线程执行任务
    /// \tparam ReturnType 返回值类型
    /// \param task 任务
    /// \return std::shared_future 对象
    template<class ReturnType>
    std::shared_future<ReturnType> async(const std::function<ReturnType()> &task) noexcept;

    /// 将任务提交到一个现有的线程池当中
    /// \tparam ReturnType 返回值类型
    /// \param pool 现有的线程池
    /// \param task 任务
    /// \return std::shared_future 对象
    template<class ReturnType>
    std::shared_future<ReturnType> async(ThreadPool &pool, const std::function<ReturnType()> &task) noexcept;

    /// 将任务提交到全局线程池当中
    /// \tparam ReturnType 返回值类型
    /// \param task 任务
    /// \return std::shared_future 对象
    template<class ReturnType>
    std::shared_future<ReturnType> asyncWithGlobalPool(const std::function<ReturnType()> &task) noexcept;

}// namespace sese

template<class ReturnType>
std::shared_future<ReturnType> sese::async(const std::function<ReturnType()> &task) noexcept {
    std::packaged_task<ReturnType()> packagedTask(task);
    std::shared_future<ReturnType> future(packagedTask.get_future());

    std::thread([&](std::packaged_task<ReturnType()> task) {
        task();
    }, std::move(packagedTask)).detach();

    return future;
}

template<class ReturnType>
std::shared_future<ReturnType> sese::async(ThreadPool &pool, const std::function<ReturnType()> &task) noexcept {
    return pool.postTask<ReturnType>(task);
}

template<class ReturnType>
std::shared_future<ReturnType> sese::asyncWithGlobalPool(const std::function<ReturnType()> &task) noexcept {
    return sese::GlobalThreadPool::postTask<ReturnType>(task);
}