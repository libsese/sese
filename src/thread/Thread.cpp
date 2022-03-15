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
    thread_local std::string currentThreadName{MAIN_THREAD_NAME};
    thread_local sese::Thread *currentThread = nullptr;

    pid_t ThreadInfo::getCurrentThreadId() noexcept {
        return currentThreadId;
    }

    const std::string &ThreadInfo::getCurrentThreadName() noexcept {
        return currentThreadName;
    }

    sese::Thread *ThreadInfo::getCurrentThread() noexcept {
        return currentThread;
    }
}// namespace sese

// Thread.class
namespace sese {

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
}// namespace sese