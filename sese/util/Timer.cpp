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

#include "sese/util/Timer.h"
#include "sese/util/Util.h"
#include "sese/thread/Locker.h"

#include <algorithm>

using sese::Locker;
using sese::Thread;
using sese::Timer;
using sese::TimerTask;

void TimerTask::cancel() noexcept {
    cancelCallback();
}

Timer::Ptr Timer::create(size_t number) noexcept {
    auto timer = MAKE_SHARED_PRIVATE(sese::Timer);
    // number 至少为 5
    timer->number = std::max<size_t>(5, number);
    timer->timerTasks = new std::list<TimerTask::Ptr>[timer->number]; // GCOVR_EXCL_LINE
    // 启动线程
    timer->thread = std::make_unique<Thread>([timer] { timer->loop(); }, "Timer");
    timer->thread->start();
    return timer;
}

Timer::~Timer() noexcept {
    if (!isShutdown) {
        shutdown();
    }
    delete[] timerTasks; // GCOVR_EXCL_LINE
}

TimerTask::Ptr Timer::delay(const std::function<void()> &callback, int64_t relative_timestamp, bool is_repeat) noexcept {
    // 初始化任务
    auto task = std::shared_ptr<TimerTask>(new TimerTask);
    task->callback = callback;
    task->sleepTimestamp = relative_timestamp;
    task->isRepeat = is_repeat;
    task->targetTimestamp = currentTimestamp + relative_timestamp;
    task->cancelCallback = [capture0 = weak_from_this(), capture1 = task->weak_from_this()] { return Timer::cancelCallback(capture0, capture1); };

    // 添加至对应轮片
    size_t index = task->targetTimestamp % number;
    mutex.lock();
    timerTasks[index].emplace_back(task);
    mutex.unlock();
    return task;
}

void Timer::shutdown() noexcept {
    isShutdown = true;
    thread->join();
}

void Timer::loop() noexcept {
    while (!isShutdown) {
        size_t index = currentTimestamp % number;
        mutex.lock();
        for (auto iterator = timerTasks[index].begin(); iterator != timerTasks[index].end();) {
            TimerTask::Ptr task = *iterator;
            // 此处条件需要长时间测试才能检测
            if (currentTimestamp == task->targetTimestamp) { // GCOVR_EXCL_LINE
                task->callback();
                iterator = timerTasks[index].erase(iterator);
                if (task->isRepeat) {
                    task->targetTimestamp = currentTimestamp + task->sleepTimestamp;
                    size_t i = task->targetTimestamp % number;
                    timerTasks[i].emplace_back(task);
                }
            }
        }
        mutex.unlock();

        sese::sleep(1);
        currentTimestamp++;
    }
}

void Timer::cancelCallback(const std::weak_ptr<Timer> &weak_timer, const std::weak_ptr<TimerTask> &weak_task) noexcept {
    auto timer = weak_timer.lock();
    auto task = weak_task.lock();
    // 此处条件为了预防万一而设，通常不会失败
    if (timer && task) { // GCOVR_EXCL_LINE
        size_t index = task->targetTimestamp % timer->number;
        timer->mutex.lock();
        timer->timerTasks[index].remove(task);
        timer->mutex.unlock();
    }
}