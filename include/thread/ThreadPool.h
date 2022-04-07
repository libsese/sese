/**
 * @file ThreadPool.h
 * @brief 线程池类
 * @date 2022年4月7日
 * @author kaoru
 */
#pragma once
#include "Config.h"
#include "Noncopyable.h"
#include "thread/Task.h"
#include "thread/Thread.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace sese {

    /**
     * @brief 线程池类
     */
    class API ThreadPool : Noncopyable {
    public:
        /**
         * 初始化线程池
         * @param threadPoolName 线程池名称（影响池内线程名称）
         * @param threads 线程数量
         */
        ThreadPool(std::string threadPoolName, size_t threads);
        ~ThreadPool();

    public:
        /**
         * 向线程池添加单个任务
         * @param task 欲执行的任务
         */
        void postTask(const Task::Ptr &task);
        /**
         * 向线程池批量添加任务
         * @param tasks 欲执行的任务集合
         */
        void postTask(const std::vector<Task::Ptr> &tasks);
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
            std::queue<Task::Ptr> tasks;
            std::atomic<bool> isShutdown{false};
        };
        std::shared_ptr<RuntimeData> data;
    };

}// namespace sese