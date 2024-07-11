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
class  GlobalThreadPool {
public:
    static void postTask(const std::function<void()> &func);

    template<class RETURN_TYPE>
    static std::shared_future<RETURN_TYPE> postTask(const std::function<RETURN_TYPE()> &func);

#ifdef SESE_PLATFORM_WINDOWS
private:
    struct Task1 {
        std::function<void()> function;
    };

    template<class RETURN_TYPE>
    struct Task2 {
        std::packaged_task<RETURN_TYPE()> packagedTask;
    };

    static DWORD WINAPI taskRunner1(LPVOID lp_param);

    template<class RETURN_TYPE>
    static DWORD WINAPI taskRunner2(LPVOID lp_parma);
#else
private:
    static SingletonPtr<ThreadPool> globalThreadPool;
#endif
};
} // namespace sese

#ifdef SESE_PLATFORM_WINDOWS

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> sese::GlobalThreadPool::postTask(const std::function<RETURN_TYPE()> &func) {
    auto task2 = new Task2<RETURN_TYPE>();
    auto packaged_task = std::packaged_task<RETURN_TYPE()>(func);
    std::shared_future<RETURN_TYPE> future(packaged_task.get_future());
    task2->packagedTask = std::move(packaged_task);
    QueueUserWorkItem(taskRunner2<RETURN_TYPE>, task2, WT_EXECUTEDEFAULT);
    return future;
}

template<class RETURN_TYPE>
DWORD sese::GlobalThreadPool::taskRunner2(LPVOID lp_parma) {
    auto task2 = (Task2<RETURN_TYPE> *) lp_parma;
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