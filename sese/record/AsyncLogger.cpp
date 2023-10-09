#include <sese/record/AsyncLogger.h>
#include <sese/record/ConsoleAppender.h>

#include <chrono>
#include <algorithm>

using namespace sese::record;
using namespace std::chrono_literals;

AsyncLogger::AsyncLogger() : Logger() {
    currentBuffer = new io::FixedBuilder(RECORD_BUFFER_SIZE);
    nextBuffer = new io::FixedBuilder(RECORD_BUFFER_SIZE);

    thread = std::make_unique<Thread>([this] { loop(); }, "AsyncLogger");
    isShutdown = false;
    thread->start();
}

AsyncLogger::~AsyncLogger() noexcept {
    isShutdown = true;
    conditionVariable.notify_one();
    thread->join();
    delete currentBuffer; // GCOVR_EXCL_LINE
    delete nextBuffer;    // GCOVR_EXCL_LINE
}

void AsyncLogger::log(const Event::Ptr &event) noexcept {
    std::unique_lock locker(mutex);
    // 内建控制台输出地不对日志等级做任何限制
    // if (builtInAppender->getLevel() <= event->getLevel()) {
    std::string content = formatter->dump(event);
    if (currentBuffer->getWriteableSize() > content.length()) {
        currentBuffer->write(content.data(), content.length());
    } else {
        buffer2Ready.push_back(currentBuffer);

        if (nextBuffer) {
            currentBuffer = nextBuffer;
            nextBuffer = nullptr;
        } else {
            currentBuffer = new io::FixedBuilder(RECORD_BUFFER_SIZE);
        }
        currentBuffer->write(content.data(), content.length());
        conditionVariable.notify_one();
    }
    // }
}

void AsyncLogger::dump(const void *buffer, size_t length) noexcept {
    std::unique_lock locker(mutex);
    if (currentBuffer->getWriteableSize() > length) {
        currentBuffer->write((const char *) buffer, length);
    } else {
        buffer2Ready.push_back(currentBuffer);

        if (nextBuffer) {
            if (nextBuffer->getSize() > length) {
                return;
            }
            currentBuffer = nextBuffer;
            nextBuffer = nullptr;
        } else {
            currentBuffer = new io::FixedBuilder(RECORD_BUFFER_SIZE);
        }
        currentBuffer->write((const char *) buffer, length);
        conditionVariable.notify_one();
    }
}

void AsyncLogger::loop() noexcept {
    auto buffer1 = new io::FixedBuilder(RECORD_BUFFER_SIZE);
    auto buffer2 = new io::FixedBuilder(RECORD_BUFFER_SIZE);
    std::vector<io::FixedBuilder *> buffer2Write;

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
        // buffer 过多，触发几率极小
        // GCOVR_EXCL_START
        if (buffer2Write.size() > 25) {
            std::for_each(buffer2Write.begin() + 2, buffer2Write.end(), [](io::FixedBuilder *buffer) {
                delete buffer;
            });
            buffer2Write.erase(buffer2Write.begin() + 2, buffer2Write.end());
        }
        // GCOVR_EXCL_STOP

        for (const auto &buffer: buffer2Write) {
            builtInAppender->dump(buffer->data(), buffer->getReadableSize());
        }
        for (auto &appender: appenderVector) {
            for (const auto &buffer: buffer2Write) {
                appender->dump(buffer->data(), buffer->getReadableSize());
            }
        }

        // 移除过多的缓冲区，避免堆积过多
        if (buffer2Write.size() > 2) {
            std::for_each(buffer2Write.begin() + 2, buffer2Write.end(), [](io::FixedBuilder *buffer) {
                delete buffer; // GCOVR_EXCL_LINE
            });
            buffer2Write.erase(buffer2Write.begin() + 2, buffer2Write.end());
            buffer2Write.resize(2);
        }

        if (!buffer1) { // NOLINT
            buffer1 = buffer2Write.back();
            buffer2Write.pop_back();
            buffer1->reset();
        }

        if (!buffer2) { // NOLINT
            buffer2 = buffer2Write.back();
            buffer2Write.pop_back();
            buffer2->reset();
        }

        buffer2Write.clear();
    }

    // 此处需要输出剩余的 buffer，触发几率较小
    // GCOVR_EXCL_START
    for (const auto &buffer: buffer2Ready) {
        builtInAppender->dump(buffer->data(), buffer->getReadableSize());
    }
    for (auto &appender: appenderVector) {
        for (const auto &buffer: buffer2Ready) {
            appender->dump(buffer->data(), buffer->getReadableSize());
        }
    }
    for (auto &&buffer: buffer2Ready) {
        delete buffer;
    }
    buffer2Ready.clear();
    // GCOVR_EXCL_STOP

    delete buffer1; // GCOVR_EXCL_LINE
    delete buffer2; // GCOVR_EXCL_LINE
}