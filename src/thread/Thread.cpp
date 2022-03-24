#include "Thread.h"

static pid_t getPid() noexcept {
#ifdef __linux__
    return syscall(__NR_gettid);
#endif
#ifdef _WIN32
    return GetCurrentThreadId();
#endif
}

// ThreadInfo.class
namespace sese {
    thread_local pid_t currentThreadId = getPid();
    thread_local std::string currentThreadName{THREAD_MAIN_NAME};
    thread_local sese::Thread *currentThread = nullptr;

    pid_t Thread::getCurrentThreadId() noexcept {
        return currentThreadId;
    }

    const std::string &Thread::getCurrentThreadName() noexcept {
        return currentThreadName;
    }

    sese::Thread *Thread::getCurrentThread() noexcept {
        return currentThread;
    }

    Thread::Thread(const std::function<void()> &function, const std::string &name) {
        this->name = name;
        this->function = function;
    }

    void Thread::start() {
        std::thread th([this] { run(this); });
        this->th = std::move(th);
    }

    void Thread::join() {
        this->th.join();
    }

    void *Thread::run(void *threadSelf) {
        currentThread = (Thread *) threadSelf;
        currentThread->id = currentThreadId;
        currentThreadName = currentThread->name;

        std::function<void()> func;
        func.swap(currentThread->function);
        func();

        return nullptr;
    }

    bool Thread::joinable() const {
        return this->th.joinable();
    }

    void Thread::detach() {
        this->th.detach();
    }
}// namespace sese