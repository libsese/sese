/**
 * @file Timer.h
 * @brief 低精度计时器类
 * @author kaoru
 * @date 2022年5月9日
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
        using Ptr = std::shared_ptr<Timer>;

        /// @brief 任务基本数据
        struct Task {
            // 实际定时时间
            uint8_t hour = 0;
            uint8_t min = 0;
            uint8_t sec = 0;

            // 对应时间轮的相对时间
            uint8_t tHour = 0;
            uint8_t tMin = 0;
            uint8_t tSec = 0;

            std::function<void()> callback;
            bool isRepeat;
        };

    public:
        explicit Timer();
        ~Timer();
        void delay(std::function<void()> callback, uint8_t sec, uint8_t min = 0, uint8_t hour = 0, bool isRepeat = false) noexcept;
        void shutdown();

    protected:
        virtual void execute(const std::function<void()> &taskCallback);

    private:
        void makeTask(Task &task, uint8_t sec, uint8_t min, uint8_t hour);
        void loop();

        Thread::Ptr thread = nullptr;
        std::mutex mutex;
        std::atomic_bool isShutdown = false;
        std::atomic<uint8_t> currentHour = 0;
        std::atomic<uint8_t> currentMin = 0;
        std::atomic<uint8_t> currentSec = 0;
        std::list<Task> timingTasks[60];
    };
}// namespace sese
