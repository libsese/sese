#include "sese/thread/Thread.h"
#include "sese/Config.h"

#ifdef __linux__
static pid_t getTid() noexcept {
    return syscall(__NR_gettid); // NOLINT
}
#endif

#ifdef _WIN32
static tid_t getTid() noexcept {
    return GetCurrentThreadId(); // NOLINT
}
#endif

#ifdef __APPLE__
#include <unistd.h>
#include <pthread/pthread.h>
static tid_t getTid() noexcept {
    uint64_t id;
    pthread_threadid_np(nullptr, &id);
    return id;
}
#endif

namespace sese {

    thread_local sese::Thread *currentThread = nullptr;

    tid_t Thread::mainId = 0;

    int32_t ThreadInitiateTask::init() noexcept {
        Thread::setCurrentThreadAsMain();
        return 0;
    }

    int32_t ThreadInitiateTask::destroy() noexcept {
        return 0;
    }

    tid_t Thread::getCurrentThreadId() noexcept {
        return currentThread ? currentThread->id : ::getTid();
    }

    const char *Thread::getCurrentThreadName() noexcept {
        auto tid = ::getTid();
        if (tid == Thread::mainId) return THREAD_MAIN_NAME;
        return currentThread ? currentThread->name.c_str() : THREAD_DEFAULT_NAME;
    }

    void Thread::setCurrentThreadAsMain() noexcept {
        if (Thread::mainId == 0) {
            Thread::mainId = ::getTid();
        }
    }

    tid_t Thread::getMainThreadId() noexcept {
        return Thread::mainId;
    }

    sese::Thread *Thread::getCurrentThread() noexcept {
        return currentThread;
    }

    Thread::Thread(const std::function<void()> &function, const std::string &name) {
        this->name = name;
        this->function = function;
    }

    void Thread::start() {
        this->th = std::move(std::thread([this] { run(this); }));
    }

    void Thread::join() {
        this->th.join();
    }

    void *Thread::run(void *threadSelf) {
        currentThread = (Thread *) threadSelf;
        currentThread->id = ::getTid();

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