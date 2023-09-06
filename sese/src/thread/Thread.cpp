#include "sese/thread/Thread.h"
#include "sese/Config.h"

#ifdef __linux__
static sese::pid_t getTid() noexcept {
    return syscall(__NR_gettid);// NOLINT
}
#endif

#ifdef _WIN32
static sese::tid_t getTid() noexcept {
    return GetCurrentThreadId();// NOLINT
}
#endif

#ifdef __APPLE__
#include <unistd.h>
#include <pthread/pthread.h>
static sese::tid_t getTid() noexcept {
    uint64_t id;
    pthread_threadid_np(nullptr, &id);
    return id;
}
#endif

namespace sese {

    thread_local std::shared_ptr<sese::Thread::RuntimeData> currentData = nullptr;

    tid_t Thread::mainId = 0;

    int32_t ThreadInitiateTask::init() noexcept {
        Thread::setCurrentThreadAsMain();
        return 0;
    }

    int32_t ThreadInitiateTask::destroy() noexcept {
        return 0;
    }

    tid_t Thread::getCurrentThreadId() noexcept {
        return currentData ? currentData->id : ::getTid();
    }

    const char *Thread::getCurrentThreadName() noexcept {
        auto tid = ::getTid();
        if (tid == Thread::mainId) return THREAD_MAIN_NAME;
        return currentData ? currentData->name.c_str() : THREAD_DEFAULT_NAME;
    }

    void Thread::setCurrentThreadAsMain() noexcept {
        if (Thread::mainId == 0) {
            Thread::mainId = ::getTid();
        }
    }

    tid_t Thread::getMainThreadId() noexcept {
        return Thread::mainId;
    }

    Thread::RuntimeData *Thread::getCurrentThreadData() noexcept {
        return currentData.get();
    }

    Thread::Thread(const std::function<void()> &function, const std::string &name) {
        this->data = std::make_shared<Thread::RuntimeData>();// GCOVR_EXCL_LINE
        this->data->name = name;                             // GCOVR_EXCL_LINE
        this->data->function = function;                     // GCOVR_EXCL_LINE
    }

    void Thread::start() {
        this->data->th = std::move(std::thread([this] { run(this->data); }));// GCOVR_EXCL_LINE
    }

    void Thread::join() {
        this->data->th.join();
    }

    void Thread::run(std::shared_ptr<RuntimeData> data) {
        currentData = data;
        currentData->id = ::getTid();

        currentData->function();
    }

    bool Thread::joinable() const {
        return this->data->th.joinable();
    }

    void Thread::detach() {
        this->data->th.detach();
    }

}// namespace sese