#pragma once

#include <sese/record/Logger.h>
#include <sese/thread/Thread.h>
#include <sese/util/FixedBuffer.h>

#include <mutex>
#include <condition_variable>

namespace sese::record {

    class API AsyncLogger : public Logger {
    public:
        AsyncLogger();

        ~AsyncLogger() noexcept override;

        void log(const Event::Ptr &event) noexcept override;

        void loop() noexcept;

    protected:
        FixedBuffer *currentBuffer;
        FixedBuffer *nextBuffer;
        // 此队列存放准备提交的 buffer
        std::vector<FixedBuffer *> buffer2Ready;

        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::atomic_bool isShutdown;
        sese::Thread::Ptr thread;
    };
}// namespace sese::record