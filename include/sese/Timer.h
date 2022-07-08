/**
 * @file Timer.h
 * @brief 低精度计时器类
 * @author kaoru
 * @date 2022年7月8日
 * @version 0.2
 */
#pragma once
#include <sese/Config.h>
#include <sese/thread/Thread.h>
#include <atomic>
#include <functional>
#include <list>
#include <mutex>

namespace sese {

    /// @brief 低精度计时器类
    class API Timer {
    public:
        /// 定时器智能指针
        using Ptr = std::shared_ptr<Timer>;

        /// @brief 任务基本数据
        struct Task {
            friend class Timer;
            /// 延时任务指针指针
            using Ptr = std::shared_ptr<Task>;

        private:
            // 实际定时时间
            int64_t sleepTimestamp = 0;
            // 对应时间轮的相对时间
            int64_t targetTimestamp = 0;

            std::function<void()> callback;
            bool isRepeat;
        };

    public:
        explicit Timer() noexcept;
        ~Timer() noexcept;
        /// 添加延时任务
        /// \param callback 回调函数
        /// \param relativeTimestamp 相对时间戳-定时时长
        /// \param isRepeat 是否重复调用
        /// \return 延时任务
        Task::Ptr delay(const std::function<void()> &callback, int64_t relativeTimestamp, bool isRepeat = false) noexcept;
        /// 取消延时任务
        /// \param task 待取消的任务
        /// \return 执行结果
        bool cancel(const Task::Ptr &task) noexcept;
        /// 结束定时器并结束相关线程
        void shutdown() noexcept;

    protected:
        /// 执行延时任务回调函数，可根据具体需要进行重载
        /// \param taskCallback 将要执行的回调函数
        virtual void execute(const std::function<void()> &taskCallback) noexcept;

    private:
        void makeTask(const Task::Ptr &task, int64_t timestamp) noexcept;
        void loop() noexcept;

        Thread::Ptr thread = nullptr;
        std::mutex mutex;
        std::atomic_bool isShutdown = false;
        std::atomic<int64_t> currentTimestamp = 0;
        std::list<Task::Ptr> timingTasks[60];
    };
}// namespace sese
