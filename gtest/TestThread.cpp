#include "sese/thread/Async.h"
#include "sese/record/LogHelper.h"
#include "sese/record/Marco.h"
#include "sese/util/Util.h"

#include "gtest/gtest.h"

#include <cmath>

using namespace std::chrono_literals;

static const char *TYPE_MAIN_THREAD = "Main Thread";
static const char *TYPE_NOT_MAIN_THREAD = "Not Main Thread";
void proc(int &num, sese::record::LogHelper &helper);

TEST(TestThread, Thread) {
    sese::record::LogHelper log;
    int num = 0;
    sese::Thread thread([&num, &log]() { proc(num, log); }, "SubThread");
    thread.start();
    if (thread.joinable()) {
        thread.join();
        log.info("num = %d", num);
    }

    log.info("Thread's name = %s, tid = %" PRIdTid, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThreadData();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    log.info("Current thread is %s", msg);
}

void proc(int &num, sese::record::LogHelper &helper) {
    helper.info("Thread's name = %s, tid = %" PRIdTid, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThreadData();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    helper.info("Current thread is %s", msg);

    ASSERT_TRUE(i != nullptr);
    num = 1;
}

TEST(TestThread, ThreadPool) {
    sese::record::LogHelper log;

    std::vector<std::function<void()>> tasks(80);
    for (int32_t i = 20; i < 100; i++) {
        std::function<void()> t = [&log, i] {
            log.info("rt: %d", i);
            std::this_thread::sleep_for(2ms);
        };
        tasks.emplace_back(t);
    }

    sese::ThreadPool pool("ThreadPool", 4);
    EXPECT_EQ(pool.getName(), std::string_view("ThreadPool"));
    EXPECT_EQ(pool.getThreads(), 4);
    pool.postTask([&log]() { log.info("start!"); });
    pool.postTask(tasks);
    pool.postTaskEx(
            [](int i, int j) {
                printf("%d\n", i + j);
            },
            10, 11
    );
    EXPECT_GT(pool.size(), 0);

    sese::sleep(2);
    EXPECT_TRUE(pool.empty());
    pool.shutdown();
}

TEST(TestThread, AutoShutdown) {
    auto pool = sese::ThreadPool("nameless", 0);
}

TEST(TestThread, MainThread) {
    sese::record::LogHelper::i("Message from main thread.");
    auto th1 = sese::Thread([] { sese::record::LogHelper::i("Message from MyThread."); }, "MyThread");
    auto th2 = sese::Thread([] { sese::record::LogHelper::i("Message from nameless Thread."); });
    auto th3 = std::thread([] { sese::record::LogHelper::i("Message from std::thread."); });
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

template<class ReturnType>
std::shared_future<ReturnType> postTask(sese::ThreadPool &pool, const std::function<ReturnType()> &task) {
    using TaskPtr = std::shared_ptr<std::packaged_task<ReturnType()>>;
    TaskPtr packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(task);
    std::shared_future<ReturnType> future(packagedTask->get_future());

    pool.postTask([task = std::move(packagedTask)]() mutable {
        (*task)();
    });

    return future;
}

TEST(TestThread, ThreadPool_Future) {
    auto pool = sese::ThreadPool("Future", 2);

    {
        int i = 1, j = 1;
        auto future = sese::async<int>(pool, [&]() {
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
        SESE_INFO("Getvalue %d", rt);
        EXPECT_EQ(rt, 2);
    }

    {
        std::string i = "114", j = "514";
        auto future = sese::async<std::string>(pool, [&]() {
            std::this_thread::sleep_for(300ms);
            return i + j;
        });
        // auto future = packagedTask.get_future();
        std::future_status status{};
        do {
            status = future.wait_for(0.1s);
            SESE_INFO("NoReady");
        } while (status != std::future_status::ready);
        auto rt = future.get();
        SESE_INFO("Getvalue %s", rt.c_str());
        EXPECT_EQ(rt, "114514");
    }
}

TEST(TestThread, Thread_Future) {
    {
        int i = 1, j = 1;
        auto future = sese::async<int>([&]() {
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
        SESE_INFO("Getvalue %d", rt);
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
    SESE_INFO("Getvalue %d", rt);
    // EXPECT_EQ(rt, 3);
}

#include <sese/thread/SpinLock.h>
#include <sese/thread/Locker.h>
#include <sese/util/StopWatch.h>

TEST(TestThread, SpinLock) {
    int i = 0;
    sese::SpinLock spinLock;
    auto proc = [&i, &spinLock]() {
        for (int n = 0; n < 10000; ++n) {
            sese::Locker locker(spinLock);
            i += 1;
        }
    };

    sese::StopWatch stopWatch;
    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    auto span = stopWatch.stop();
    SESE_DEBUG("cost time %" PRIu64 "us", span.getTotalMicroseconds());
    EXPECT_EQ(i, 40000);
}

TEST(TestThread, Mutex) {
    int i = 0;
    std::mutex mutex;
    auto proc = [&i, &mutex]() {
        for (int n = 0; n < 10000; ++n) {
            sese::Locker locker(mutex);
            i += 1;
        }
    };

    sese::StopWatch stopWatch;
    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    auto span = stopWatch.stop();
    SESE_DEBUG("cost time %" PRIu64 "us", span.getTotalMicroseconds());
    EXPECT_EQ(i, 40000);
}