#include <sese/Timer.h>
#include <sese/Util.h>
#include <sese/thread/Locker.h>

using sese::Locker;
using sese::Thread;
using sese::Timer;
using Task = sese::Timer::Task;

Timer::Timer() noexcept {
    thread = std::make_unique<Thread>([this] { loop(); }, "Timer");
    thread->start();
}

Timer::~Timer() noexcept {
    if (!isShutdown) {
        shutdown();
    }
}

Task::Ptr Timer::delay(const std::function<void()> &callback, int64_t relativeTimestamp, bool isRepeat) noexcept {
    auto task = std::make_shared<Task>();
    task->sleepTimestamp = relativeTimestamp;
    makeTask(task, relativeTimestamp);
    task->callback = callback;
    task->isRepeat = isRepeat;

    Locker locker(mutex);
    timingTasks[task->targetTimestamp % 60].emplace_back(task);
    return task;
}

bool Timer::cancel(const Task::Ptr &task) noexcept {
    Locker locker(mutex);
    // CXX 20 Only
    // return 0 != std::erase(timingTasks[task->tSec], task);
    auto sec = task->targetTimestamp % 60;
    for (auto iterator = timingTasks[sec].begin();
         iterator != timingTasks[sec].end();
         iterator++) {
        if (*iterator == task) {
            timingTasks[sec].erase(iterator);
            return true;
        }
    }
    return false;
}

void Timer::shutdown() noexcept {
    isShutdown = true;
    thread->join();
}

void Timer::execute(const std::function<void()> &taskCallback) noexcept {
    taskCallback();
}

void Timer::makeTask(const Task::Ptr &task, int64_t timestamp) noexcept {
    task->targetTimestamp = currentTimestamp + timestamp;
}

void Timer::loop() noexcept {
    while (!isShutdown) {
        sese::sleep(1);

        // 检查任务
        auto sec = currentTimestamp % 60;
        auto &currentTimingTask = timingTasks[sec];
        mutex.lock();
        if (!currentTimingTask.empty()) {
            for (auto iterator = currentTimingTask.begin();
                 iterator != currentTimingTask.end();) {
                auto task = *iterator;
                if (currentTimestamp == (*iterator)->targetTimestamp) {
                    execute(task->callback);
                    if (task->isRepeat) {
                        // 重复任务再次添加
                        makeTask(task, task->sleepTimestamp);
                        timingTasks[task->targetTimestamp % 60].emplace_back(task);
                    }
                    iterator = currentTimingTask.erase(iterator);
                }
            }
        }
        mutex.unlock();
        currentTimestamp++;
    }
}