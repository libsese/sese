// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/thread/Async.h"
#include "sese/log/Marco.h"
#include "sese/util/Util.h"

#include "gtest/gtest.h"

#include <cmath>

using namespace std::chrono_literals;
using sese::log::Logger;

static auto type_main_thread = "Main Thread";
static auto type_not_main_thread = "Not Main Thread";

void proc(int &num);

TEST(TestThread, Thread) {
    int num = 0;
    sese::Thread thread([&num] {
        proc(num);
    },
                        "SubThread");
    thread.start();
    if (thread.joinable()) {
        thread.join();
        Logger::debug("num = {}", num);
    }

    Logger::debug("Thread's name = {}, tid = {}", sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThreadData();
    auto msg = i ? type_not_main_thread : type_main_thread;
    Logger::debug("Current thread is {}", msg);
}

void proc(int &num) {
    Logger::debug("Thread's name = {}, tid = {}", sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThreadData();
    auto msg = i ? type_not_main_thread : type_main_thread;
    Logger::debug("Current thread is {}", msg);

    ASSERT_TRUE(i != nullptr);
    num = 1;
}

TEST(TestThread, ThreadPool) {
    std::vector<std::function<void()>> tasks(80);
    for (int32_t i = 20; i < 100; i++) {
        std::function<void()> t = [i] {
            Logger::debug("rt: {}", i);
            std::this_thread::sleep_for(2ms);
        };
        tasks.emplace_back(t);
    }

    sese::ThreadPool pool("ThreadPool", 4);
    EXPECT_EQ(pool.getName(), std::string_view("ThreadPool"));
    EXPECT_EQ(pool.getThreads(), 4);
    pool.postTask([] {
        Logger::debug("start!");
    });
    pool.postTask(tasks);
    pool.postTaskEx(
        [](int i, int j) {
            printf("%d\n", i + j);
        },
        10, 11
    );
    EXPECT_GT(pool.size(), 0);

    sese::sleep(2s);
    EXPECT_TRUE(pool.empty());
    pool.shutdown();
}

TEST(TestThread, AutoShutdown) {
    auto pool = sese::ThreadPool("nameless", 0);
}

TEST(TestThread, AutoJoin) {
    int i = 0;
    {
        sese::Thread th([&i] {
            sese::sleep(2s);
            i = 114514;
        });
        th.start();
    }
    ASSERT_EQ(i, 114514);
}

TEST(TestThread, MainThread) {
    Logger::info("Message from main thread.");
    auto th1 = sese::Thread([] {
        Logger::info("Message from MyThread.");
    },
                            "MyThread");
    auto th2 = sese::Thread([] {
        Logger::info("Message from nameless Thread.");
    });
    auto th3 = std::thread([] {
        Logger::info("Message from std::thread.");
    });
    th1.start();
    th2.start();
    th1.join();
    th2.detach();
    th3.join();

    auto th4 = sese::Thread(
        [] {
            sese::Thread::setCurrentThreadAsMain();
            ASSERT_NE(sese::Thread::getCurrentThreadId(), sese::Thread::getMainThreadId());
        },
        "Sub thread"
    );
    th4.start();
    th4.join();

    std::this_thread::sleep_for(100ms);
}

void func(std::packaged_task<int()> task) {
    task();
}

template<class RETURN_TYPE>
std::shared_future<RETURN_TYPE> postTask(sese::ThreadPool &pool, const std::function<RETURN_TYPE()> &task) {
    using TaskPtr = std::shared_ptr<std::packaged_task<RETURN_TYPE()>>;
    TaskPtr packaged_task = std::make_shared<std::packaged_task<RETURN_TYPE()>>(task);
    std::shared_future<RETURN_TYPE> future(packaged_task->get_future());

    pool.postTask([task = std::move(packaged_task)]() mutable {
        (*task)();
    });

    return future;
}

TEST(TestThread, ThreadPool_Future) {
    auto pool = sese::ThreadPool("Future", 2);

    {
        int i = 1, j = 1;
        auto future = sese::async<int>(pool, [&] {
            // auto future = sese::async<int>(pool, [&]() {
            std::this_thread::sleep_for(300ms);
            SESE_INFO("SetValue");
            return i + j;
        });
        // auto future = packagedTask.get_future();
        std::future_status status{};
        do {
            status = future.wait_for(0.1s);
            SESE_INFO("NoReady");
        } while (status != std::future_status::ready);
        auto rt = future.get();
        SESE_INFO("Getvalue {}", rt);
        EXPECT_EQ(rt, 2);
    }

    {
        std::string i = "114", j = "514";
        auto future = sese::async<std::string>(pool, [&] {
            std::this_thread::sleep_for(300ms);
            return i + j;
        });
        // auto future = packagedTask.get_future();
        std::future_status status{};
        do {
            status = future.wait_for(0.1s);
            SESE_INFO("NoReady");
        } while (status != std::future_status::ready);
        const auto &rt = future.get();
        SESE_INFO("Getvalue {}", rt.c_str());
        EXPECT_EQ(rt, "114514");
    }
}

TEST(TestThread, Thread_Future) {
    {
        int i = 1, j = 1;
        auto future = sese::async<int>([&] {
            std::this_thread::sleep_for(200ms);
            SESE_INFO("SetValue");
            return i + j;
        });
        // auto future = packagedTask.get_future();
        std::future_status status{};
        do {
            status = future.wait_for(0.1s);
            SESE_INFO("NoReady");
        } while (status != std::future_status::ready);
        auto rt = future.get();
        SESE_INFO("Getvalue {}", rt);
        // EXPECT_EQ(rt, 2);
    }
}

#include <sese/thread/GlobalThreadPool.h>

TEST(TestThread, GlobalThreadPool) {
    sese::GlobalThreadPool::postTask([] {
        SESE_INFO("Hello World");
    });

    int i = 1, j = 2;
    auto future = sese::asyncWithGlobalPool<int>([&]() -> int {
        std::this_thread::sleep_for(300ms);
        SESE_INFO("SetValue");
        return i + j;
    });
    std::future_status status{};
    do {
        status = future.wait_for(0.1s);
        SESE_INFO("NoReady");

    } while (status != std::future_status::ready);
    auto rt = future.get();
    SESE_INFO("Getvalue {}", rt);
    // EXPECT_EQ(rt, 3);
}

#include <sese/thread/SpinLock.h>
#include <sese/thread/Locker.h>
#include <sese/util/StopWatch.h>

TEST(TestThread, SpinLock) {
    int i = 0;
    sese::SpinLock spin_lock;
    auto proc = [&i, &spin_lock] {
        for (int n = 0; n < 10000; ++n) {
            sese::Locker locker(spin_lock);
            i += 1;
        }
    };

    sese::StopWatch stop_watch;
    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    auto span = stop_watch.stop();
    SESE_DEBUG("cost time {}us", span.getTotalMicroseconds());
    EXPECT_EQ(i, 40000);
}

TEST(TestThread, Mutex) {
    int i = 0;
    std::mutex mutex;
    auto proc = [&i, &mutex] {
        for (int n = 0; n < 10000; ++n) {
            sese::Locker locker(mutex);
            i += 1;
        }
    };

    sese::StopWatch stop_watch;
    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    auto span = stop_watch.stop();
    SESE_DEBUG("cost time {}us", span.getTotalMicroseconds());
    EXPECT_EQ(i, 40000);
}