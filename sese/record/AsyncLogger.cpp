// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    if (builtInAppender->getLevel() <= event->getLevel()) {
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
    }
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
    std::vector<io::FixedBuilder *> buffer2_write;

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
            buffer2_write.swap(buffer2Ready);
            if (!nextBuffer) {
                nextBuffer = buffer2;
                buffer2 = nullptr;
            }
        }
        // buffer 过多，触发几率极小
        // GCOVR_EXCL_START
        if (buffer2_write.size() > 25) {
            std::for_each(buffer2_write.begin() + 2, buffer2_write.end(), [](io::FixedBuilder *buffer) {
                delete buffer;
            });
            buffer2_write.erase(buffer2_write.begin() + 2, buffer2_write.end());
        }
        // GCOVR_EXCL_STOP

        for (const auto &buffer: buffer2_write) {
            builtInAppender->dump(buffer->data(), buffer->getReadableSize());
        }
        for (auto &appender: appenderVector) {
            for (const auto &buffer: buffer2_write) {
                appender->dump(buffer->data(), buffer->getReadableSize());
            }
        }

        // 移除过多的缓冲区，避免堆积过多
        if (buffer2_write.size() > 2) {
            std::for_each(buffer2_write.begin() + 2, buffer2_write.end(), [](io::FixedBuilder *buffer) {
                delete buffer; // GCOVR_EXCL_LINE
            });
            buffer2_write.erase(buffer2_write.begin() + 2, buffer2_write.end());
            buffer2_write.resize(2);
        }

        if (!buffer1) { // NOLINT
            buffer1 = buffer2_write.back();
            buffer2_write.pop_back();
            buffer1->reset();
        }

        if (!buffer2) { // NOLINT
            buffer2 = buffer2_write.back();
            buffer2_write.pop_back();
            buffer2->reset();
        }

        buffer2_write.clear();
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