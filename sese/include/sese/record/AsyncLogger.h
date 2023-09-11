/// \file AsyncLogger.h
/// \author kaoru
/// \date 2023年6月11日
/// \brief 异步日志器
/// \version 0.1.0
/// \note 此处使用 valgrind 测试必定产生内存泄漏，使用 -fsanitize=leak 选项进行测试则一切正常

#pragma once

#include <sese/record/Logger.h>
#include <sese/thread/Thread.h>
#include <sese/io/FixedBuilder.h>

#include <atomic>
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
        io::FixedBuilder *currentBuffer;
        io::FixedBuilder *nextBuffer;
        // 此队列存放准备提交的 buffer
        std::vector<io::FixedBuilder *> buffer2Ready;

        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::atomic_bool isShutdown{};
        sese::Thread::Ptr thread;
    };
}// namespace sese::record