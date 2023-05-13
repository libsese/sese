#include <sese/record/AsyncLogger.h>
#include <sese/record/ConsoleAppender.h>
#include <sese/thread/Locker.h>

#include <chrono>

using namespace sese::record;
using namespace std::chrono_literals;

AsyncLogger::AsyncLogger() : Logger() {
}

AsyncLogger::~AsyncLogger() noexcept {
}

void AsyncLogger::log(const Event::Ptr &event) noexcept {
    std::unique_lock locker(mutex);
}

void AsyncLogger::loop() noexcept {
    auto buffer1 = new Buffer;
    auto buffer2 = new Buffer;
    std::vector<Buffer *> buffer2Write;

    while (true) {
        {
            std::unique_lock locker(mutex);
            if (buffers.empty()) {
                std::this_thread::sleep_for(3s);
                continue;
            }
            buffers.push_back(std::move(currentBuffer));
            currentBuffer = std::move(buffer1);
            buffer2Write.swap(buffers);
            if (!nextBuffer) {
                nextBuffer = std::move(buffer2);
            }
        }

        if (buffer2Write.size() > 25) {
            // buffer 过多
        }
        for (const auto &buffer: buffer2Write) {
            builtInAppender->dump((const char *) buffer->raw, buffer->len);
        }
        if (buffer2Write.size() > 2) {
            // 移除过多的缓冲区，避免堆积过多
            buffer2Write.resize(2);
        }

        if (!buffer1) {
            buffer1 = std::move(buffer2Write.back());
            buffer2Write.pop_back();
        }

        if (!buffer2) {
            buffer2 = std::move(buffer2Write.back());
            buffer2Write.pop_back();
        }

        buffer2Write.clear();
    }
}