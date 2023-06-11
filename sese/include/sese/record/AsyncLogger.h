/// \file AsyncLogger.h
/// \author kaoru
/// \date 2023年6月11日
/// \brief 异步日志器
/// \version 0.1.0

#pragma once

#include <sese/record/Logger.h>
#include <sese/thread/Thread.h>
#include <sese/util/FixedBuilder.h>

#include <mutex>
#include <condition_variable>

namespace sese::record {

    class API AsyncLogger final : public Logger {
    public:
        AsyncLogger();

        ~AsyncLogger() noexcept override;

        void log(const Event::Ptr &event) noexcept override;

        void loop() noexcept;

    protected:
        FixedBuilder *currentBuffer;
        FixedBuilder *nextBuffer;
        // 此队列存放准备提交的 buffer
        std::vector<FixedBuilder *> buffer2Ready;

        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::atomic_bool isShutdown;
        sese::Thread::Ptr thread;
    };
}// namespace sese::record