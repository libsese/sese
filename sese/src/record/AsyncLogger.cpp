#include <sese/record/AsyncLogger.h>
#include <sese/record/ConsoleAppender.h>

#include <chrono>

using namespace sese::record;
using namespace std::chrono_literals;

AsyncLogger::AsyncLogger() : Logger() {
    currentBuffer = new FixedBuffer(RECORD_BUFFER_SIZE);
    nextBuffer = new FixedBuffer(RECORD_BUFFER_SIZE);

    thread = std::make_unique<Thread>([this] { loop(); }, "AsyncLogger");
    isShutdown = false;
    thread->start();
}

AsyncLogger::~AsyncLogger() noexcept {
    isShutdown = true;
    conditionVariable.notify_one();
    thread->join();
    delete currentBuffer;
    delete nextBuffer;
}

void AsyncLogger::log(const Event::Ptr &event) noexcept {
    std::unique_lock locker(mutex);
    if (builtInAppender->getLevel() <= event->getLevel()) {
        std::string content = formatter->dump(event);
        if (currentBuffer->getWriteableSize() > content.length() + 1) {
            currentBuffer->write(content.data(), content.length());
            currentBuffer->write("\n", 1);
        } else {
            buffer2Ready.push_back(currentBuffer);

            if (nextBuffer) {
                currentBuffer = nextBuffer;
                nextBuffer = nullptr;
            } else {
                currentBuffer = new FixedBuffer(RECORD_BUFFER_SIZE);
            }
            currentBuffer->write(content.data(), content.length());
            currentBuffer->write("\n", 1);
            conditionVariable.notify_one();
        }
    }
}

void AsyncLogger::loop() noexcept {
    auto buffer1 = new FixedBuffer(RECORD_BUFFER_SIZE);
    auto buffer2 = new FixedBuffer(RECORD_BUFFER_SIZE);
    std::vector<FixedBuffer *> buffer2Write;

    while (true) {
        if (isShutdown) break;
        {
            std::unique_lock locker(mutex);
            if (buffer2Ready.empty()) {
                conditionVariable.wait_for(locker, 3s);
            }
            buffer2Ready.emplace_back(currentBuffer);
            currentBuffer = buffer1;
            buffer1 = nullptr;
            buffer2Write.swap(buffer2Ready);
            if (!nextBuffer) {
                nextBuffer = buffer2;
                buffer2 = nullptr;
            }
        }

        if (buffer2Write.size() > 25) {
            // buffer 过多
            std::for_each(buffer2Write.begin() + 2, buffer2Write.end(), [](FixedBuffer *buffer) {
                delete buffer;
            });
            buffer2Write.erase(buffer2Write.begin() + 2, buffer2Write.end());
        }
        for (const auto &buffer: buffer2Write) {
            builtInAppender->dump(buffer->data(), buffer->getReadableSize());
        }
        if (buffer2Write.size() > 2) {
            // 移除过多的缓冲区，避免堆积过多
            std::for_each(buffer2Write.begin() + 2, buffer2Write.end(), [](FixedBuffer *buffer) {
                delete buffer;
            });
            buffer2Write.erase(buffer2Write.begin() + 2, buffer2Write.end());
            buffer2Write.resize(2);
        }

        if (!buffer1) {
            buffer1 = buffer2Write.back();
            buffer2Write.pop_back();
            buffer1->reset();
        }

        if (!buffer2) {
            buffer2 = buffer2Write.back();
            buffer2Write.pop_back();
            buffer2->reset();
        }

        buffer2Write.clear();
    }

    delete buffer1;
    delete buffer2;
}