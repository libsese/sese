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
class API ThreadPool : public Noncopyable {
public:
    using Ptr = std::unique_ptr<ThreadPool>;

    /**
     * 初始化线程池
     * @param threadPoolName 线程池名称（影响池内线程名称）
     * @param threads 线程数量
     */
    explicit ThreadPool(std::string threadPoolName = THREAD_DEFAULT_NAME, size_t threads = 4);
    ~ThreadPool();

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
     * @tparam Function 函数模板
     * @tparam Args 参数模板
     * @param f 函数
     * @param args 参数
     */
    template<typename Function, typename... Args>
    void postTaskEx(Function &&f, Args &&...args) {
        auto boundFunction = [func = std::forward<Function>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            std::apply(std::move(func), std::move(args));
        };
        this->postTask(std::move(boundFunction));
    }

    /**
     * 向线程池添加有返回值的任务
     * \tparam ReturnType 返回值类型
     * \param tasks 欲执行的任务
     * \return std::shared_future 对象
     */
    template<class ReturnType>
    std::shared_future<ReturnType> postTask(const std::function<ReturnType()> &tasks);

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

template<class ReturnType>
std::shared_future<ReturnType> sese::ThreadPool::postTask(const std::function<ReturnType()> &task) {
    /**
     * 注意：
     * 由于 std::packaged_task 属于不可拷贝对象，
     * 并且 std::function 会对参数类型进行擦除，导致 std::move 也无法作用于不可拷贝对象，
     * 所以此处选择了使用 std::shared_ptr 对 std::packaged_task 进行封装
     */
    using TaskPtr = std::shared_ptr<std::packaged_task<ReturnType()>>;
    TaskPtr packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(task);
    std::shared_future<ReturnType> future(packagedTask->get_future());

    this->postTask([task = std::move(packagedTask)]() mutable {
        (*task)();
    });

    return future;
}

// GCOVR_EXCL_STOP