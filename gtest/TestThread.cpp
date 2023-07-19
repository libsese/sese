#include "sese/thread/Thread.h"
#include "sese/thread/ThreadPool.h"
#include "sese/record/LogHelper.h"
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

    auto i = sese::Thread::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    log.info("Current thread is %s", msg);
}

void proc(int &num, sese::record::LogHelper &helper) {
    helper.info("Thread's name = %s, tid = %" PRIdTid, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThread();
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
            std::this_thread::sleep_for(20ms);
        };
        tasks.emplace_back(t);
    }

    sese::ThreadPool pool("threadPool", 4);
    ASSERT_EQ(pool.getName(), std::string_view("threadPool"));
    ASSERT_EQ(pool.getThreads(), 4);
    pool.postTask([&log]() { log.info("start!"); });
    pool.postTask(tasks);
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
}