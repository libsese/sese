/**
 * @file Timer.h
 * @brief 低精度计时器类
 * @author kaoru
 * @date 2022年7月11日
 * @version 0.3
 */
#pragma once
#include <sese/Config.h>
#include <sese/thread/Thread.h>
#include <atomic>
#include <functional>
#include <list>
#include <mutex>

namespace sese {

    class API Timer;
    /// 定时任务
    class API TimerTask final {
    public:
        friend class Timer;
        using Ptr = std::shared_ptr<TimerTask>;

        /// 取消当前定时任务
        void cancel() noexcept;

    private:
        // 私有构造函数
        TimerTask() = default;
        // 对应定时长度
        int64_t sleepTimestamp = 0;
        // 对应定时器的时间
        int64_t targetTimestamp = 0;
        // 是否重复执行
        bool isRepeat = false;
        // 定时回调函数
        std::function<void()> callback;
        // 取消回调函数
        std::function<void()> cancelCallback;
    };

    /// 低精度计时器类
    class API Timer final : public std::enable_shared_from_this<Timer> {
    public:
        using Ptr = std::shared_ptr<Timer>;

        /// 初始化一个定时器
        /// \param number 定时器的时间轮轮片数量
        static Timer::Ptr create(size_t number = TIMER_WHEEL_NUMBER) noexcept;
        /// 定时器析构
        ~Timer() noexcept;

        /// 设定延时任务
        /// \param callback 回调函数
        /// \param relativeTimestamp 定时时间
        /// \param isRepeat 是否重复
        /// \return 任务句柄
        TimerTask::Ptr delay(const std::function<void()> &callback, int64_t relativeTimestamp, bool isRepeat = false) noexcept;
        /// 结束定时器并终止定时线程
        void shutdown() noexcept;

    private:
        /// 私有构造函数
        explicit Timer(size_t number);
        void loop() noexcept;
        static void cancelCallback(const std::weak_ptr<Timer> &weakTimer, TimerTask::Ptr task) noexcept;

        size_t number = 0;
        std::mutex mutex{};
        Thread::Ptr thread = nullptr;
        std::atomic_bool isShutdown = false;
        std::atomic<int64_t> currentTimestamp = 0;
        std::list<TimerTask::Ptr> *timerTasks = nullptr;
    };
}// namespace sese
