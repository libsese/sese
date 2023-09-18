/// \file GlobalThreadPool.h
/// \author kaoru
/// \brief 全局线程池
/// \date 2023年8月6日

#pragma once

#include <sese/Config.h>
#include <sese/thread/ThreadPool.h>
#include <sese/util/Singleton.h>

namespace sese {

    /// 全局线程池，在 Windows 上使用系统线程池，而在 UNIX 平台上则使用懒汉单例模式的线程池
    class API GlobalThreadPool {
    public:
        static void postTask(const std::function<void()> &func);

        template<class ReturnType>
        static std::shared_future<ReturnType> postTask(const std::function<ReturnType()> &func);

#ifdef SESE_PLATFORM_WINDOWS
    private:
        struct Task1 {
            std::function<void()> function;
        };

        template<class ReturnType>
        struct Task2 {
            std::packaged_task<ReturnType()> packagedTask;
        };

        static DWORD WINAPI taskRunner1(LPVOID lpParam);

        template<class ReturnType>
        static DWORD WINAPI taskRunner2(LPVOID lpParma);
#else
    private:
        static SingletonPtr<ThreadPool> globalThreadPool;
#endif
    };
}// namespace sese

#ifdef SESE_PLATFORM_WINDOWS

template<class ReturnType>
std::shared_future<ReturnType> sese::GlobalThreadPool::postTask(const std::function<ReturnType()> &func) {
    auto task2 = new Task2<ReturnType>();
    auto packagedTask = std::packaged_task<ReturnType()>(func);
    std::shared_future<ReturnType> future(packagedTask.get_future());
    task2->packagedTask = std::move(packagedTask);
    QueueUserWorkItem(taskRunner2<ReturnType>, task2, WT_EXECUTEDEFAULT);
    return future;
}

template<class ReturnType>
DWORD sese::GlobalThreadPool::taskRunner2(LPVOID lpParma) {
    auto task2 = (Task2<ReturnType> *) lpParma;
    task2->packagedTask();
    delete task2;
    return 0;
}

#else

template<class ReturnType>
std::shared_future<ReturnType> sese::GlobalThreadPool::postTask(const std::function<ReturnType()> &func) {
    auto pool = globalThreadPool.getInstance();
    return pool->postTask<ReturnType>(func);
}

#endif