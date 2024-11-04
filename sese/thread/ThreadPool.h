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
 * @brief 线程池类
 * @date 2022年4月7日
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
 * @brief 线程池类
 */
class  ThreadPool : public Noncopyable {
public:
    using Ptr = std::unique_ptr<ThreadPool>;

    /**
     * 初始化线程池
     * @param thread_pool_name 线程池名称（影响池内线程名称）
     * @param threads 线程数量
     */
    explicit ThreadPool(std::string thread_pool_name = THREAD_DEFAULT_NAME, size_t threads = 4);
    ~ThreadPool() override;

public:
    /**
     * 向线程池添加单个任务
     * @param task 欲执行的任务
     */
    void postTask(const std::function<void()> &task);

    /**
     * 向线程池批量添加任务
     * @param tasks 欲执行的任务集合
     */
    void postTask(const std::vector<std::function<void()>> &tasks);

    /**
     * 向线程池添加任务并绑定参数
     * @tparam FUNCTION 函数模板
     * @tparam ARGS 参数模板
     * @param f 函数
     * @param args 参数
     */
    template<typename FUNCTION, typename... ARGS>
    void postTaskEx(FUNCTION &&f, ARGS &&...args) {
        auto bound_function = [func = std::forward<FUNCTION>(f), args = std::make_tuple(std::forward<ARGS>(args)...)]() mutable {
            std::apply(std::move(func), std::move(args));
        };
        this->postTask(std::move(bound_function));
    }

    /**
     * 向线程池添加有返回值的任务
     * \tparam RETURN_TYPE 返回值类型
     * \param tasks 欲执行的任务
     * \return std::shared_future 对象
     */
    template<class RETURN_TYPE>
    std::shared_future<RETURN_TYPE> postTask(const std::function<RETURN_TYPE()> &tasks);

    /**
     * @brief 关闭当前线程池并阻塞至子线程退出
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

    /// 线程池运行数据
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
     * 注意：
     * 由于 std::packaged_task 属于不可拷贝对象，
     * 并且 std::function 会对参数类型进行擦除，导致 std::move 也无法作用于不可拷贝对象，
     * 所以此处选择了使用 std::shared_ptr 对 std::packaged_task 进行封装
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