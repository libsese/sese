#include <sese/Timer.h>
#include <sese/Util.h>
#include <sese/thread/Locker.h>

using sese::Locker;
using sese::Thread;
using sese::Timer;
using sese::TimerTask;

void TimerTask::cancel() noexcept {
    cancelCallback();
}

Timer::Ptr Timer::create(size_t number) noexcept {
    return std::shared_ptr<Timer>(new Timer(number));
}

Timer::~Timer() noexcept {
    if (!isShutdown) {
        shutdown();
    }
    delete[] timerTasks;
}

TimerTask::Ptr Timer::delay(const std::function<void()> &callback, int64_t relativeTimestamp, bool isRepeat) noexcept {
    // 初始化任务
    auto task = std::shared_ptr<TimerTask>(new TimerTask);
    task->callback = callback;
    task->sleepTimestamp = relativeTimestamp;
    task->isRepeat = isRepeat;
    task->targetTimestamp = currentTimestamp + relativeTimestamp;
    task->cancelCallback = [capture0 = weak_from_this(), task] { return Timer::cancelCallback(capture0, task); };

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

Timer::Timer(size_t number) {
    // number 至少为 1
    this->number = number == 0 ? 1 : number;
    timerTasks = new std::list<TimerTask::Ptr>[number];
    // 启动线程
    thread = std::make_unique<Thread>([this] { loop(); }, "Timer");
    thread->start();
}

void Timer::loop() noexcept {
    while(!isShutdown) {
        size_t index = currentTimestamp % number;
        mutex.lock();
        for(auto iterator = timerTasks[index].begin(); iterator != timerTasks[index].end();) {
            TimerTask::Ptr task = *iterator;
            if(currentTimestamp == task->targetTimestamp) {
                task->callback();
                iterator = timerTasks[index].erase(iterator);
                if(task->isRepeat) {
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

void Timer::cancelCallback(const std::weak_ptr<Timer> &weakTimer, TimerTask::Ptr task) noexcept {
    auto timer = weakTimer.lock();
    if(timer) {
        size_t index = task->targetTimestamp % timer->number;
        timer->mutex.lock();
        timer->timerTasks[index].remove(task);
        timer->mutex.unlock();
    }
}