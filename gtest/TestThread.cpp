#include "sese/thread/Thread.h"
#include "sese/thread/ThreadPool.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Util.h"
#include "gtest/gtest.h"

static const char *TYPE_MAIN_THREAD = "Main Thread";
static const char *TYPE_NOT_MAIN_THREAD = "Not Main Thread";
void proc(int &num, sese::record::LogHelper &helper);

TEST(TestThread, Thread) {
    sese::record::LogHelper log("Thread");
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
    sese::record::LogHelper log("ThreadPool");

    auto task = [&log = log](int value){
        log.info("rt: %f", std::tgamma(value));
    };

    std::vector<std::function<void ()> > tasks(80);
    for (int32_t i = 20; i < 100; i++) {
        std::function<void ()> t = [task, i] { return task(i); };
        tasks.emplace_back(t);
    }

    sese::ThreadPool pool("threadPool", 4);
    pool.postTask(tasks);

    sese::sleep(2);
    pool.shutdown();
}