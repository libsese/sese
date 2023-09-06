/**
 * @file Thread.h
 * @brief 线程类
 * @author kaoru
 * @date 2022年3月28日
 * @bug 使用线程的分离功能处理不当有可能导致内存泄漏，对于分离线程这种情况推荐考虑使用标准库线程
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/Initializer.h"
#include "sese/util/Noncopyable.h"

#include <functional>
#include <thread>

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable : 4251)
#pragma warning(disable : 4624)
#endif

namespace sese {

    class ThreadInitiateTask final : public InitiateTask {
    public:
        ThreadInitiateTask() : InitiateTask(__FUNCTION__){};

        int32_t init() noexcept override;

        int32_t destroy() noexcept override;
    };

    /**
     * @brief 线程类
     */
    class API Thread : Noncopyable {
    public:
        using Ptr = std::unique_ptr<Thread>;

        struct RuntimeData;

        explicit Thread(const std::function<void()> &function, const std::string &name = THREAD_DEFAULT_NAME);

        void start();
        void join();
        void detach();
        [[nodiscard]] bool joinable() const;

        static void run(std::shared_ptr<RuntimeData> data);

        [[nodiscard]] tid_t getTid() const noexcept { return data->id; }
        [[nodiscard]] const std::string &getThreadName() const noexcept { return this->data->name; }

    public:
        struct RuntimeData {
            tid_t id = 0;

            std::string name;
            std::thread th;
            std::function<void()> function;
        };

        static tid_t getCurrentThreadId() noexcept;
        static const char *getCurrentThreadName() noexcept;
        static void setCurrentThreadAsMain() noexcept;
        static tid_t getMainThreadId() noexcept;

        /**
         * 获取当前线程实例
         * @return 当前线程实例，当前线程为主线程时返回 nullptr
         */
        static Thread::RuntimeData *getCurrentThreadData() noexcept;

    private:
        static tid_t mainId;
        std::shared_ptr<RuntimeData> data = nullptr;
    };

}// namespace sese