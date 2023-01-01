#include "sese/thread/Thread.h"

#ifdef __linux__
static pid_t getTid() noexcept {
    return syscall(__NR_gettid);
}
#endif

#ifdef _WIN32
static tid_t getTid() noexcept {
    return GetCurrentThreadId();
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

    tid_t Thread::getCurrentThreadId() noexcept {
        return currentThread ? currentThread->id : ::getTid();
    }

    const char *Thread::getCurrentThreadName() noexcept {
        return currentThread ? currentThread->name.c_str() : THREAD_MAIN_NAME;
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