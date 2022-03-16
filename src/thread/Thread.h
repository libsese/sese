#pragma once
#include "Config.h"
#include "util/Noncopyable.h"
#include "util/NotInstantiable.h"
#include <functional>
#include <thread>

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#pragma warning(disable : 4251)
#pragma warning(disable : 4624)
#endif

namespace sese {

    typedef void *ThreadArgumentPtr;

    class API Thread : Noncopyable {
    public:
        explicit Thread(const std::function<void()> &function, const std::string &name = DEFAULT_THREAD_NAME);
        ~Thread() = default;

        void start();
        void join();
        [[nodiscard]] bool joinable() const;
        void detach();
        void *run(void *threadSelf);

        [[nodiscard]] pid_t getPid() const noexcept { return this->id; }
        [[nodiscard]] const std::string &getThreadName() const noexcept { return this->name; }
        [[nodiscard]] ThreadArgumentPtr getArgument() const { return this->argument; }
        void setArgument(ThreadArgumentPtr myArgument) { this->argument = myArgument; }

    private:
        std::string name;
        pid_t id = -1;
        std::thread th;
        std::function<void()> function;
        ThreadArgumentPtr argument = nullptr;
    };

    class API ThreadInfo : NotInstantiable {
    public:
        static pid_t getCurrentThreadId() noexcept;
        static const std::string &getCurrentThreadName() noexcept;
        /**
         * 获取当前线程实例
         * @return 当前线程实例，当前线程为主线程时返回 nullptr
         */
        static Thread *getCurrentThread() noexcept;
    };

}// namespace sese