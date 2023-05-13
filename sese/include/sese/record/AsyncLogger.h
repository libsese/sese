#pragma once

#include <sese/record/Logger.h>
#include <sese/util/Timer.h>

namespace sese::record {

    struct Buffer {
        uint8_t raw[RECORD_BUFFER_SIZE]{};
        size_t len = 0;
    };

    class API AsyncLogger : public Logger {
    public:
        AsyncLogger();

        ~AsyncLogger() noexcept override;

        void log(const Event::Ptr &event) noexcept override;

        void loop() noexcept;

    protected:
        Buffer *currentBuffer;
        Buffer *nextBuffer;
        std::mutex mutex;
        std::condition_variable conditionVariable;
        sese::Thread::Ptr thread;
        std::vector<Buffer *> buffers;
    };
}// namespace sese::record