#include <sese/record/LogHelper.h>
#include <sese/record/Marco.h>
#include <sese/thread/Thread.h>
#include <sese/util/Timer.h>
#include <sese/util/TimeWheel.h>
#include <sese/util/Util.h>

#include <gtest/gtest.h>

void foo() {
    SESE_DEBUG("foo time up");
}

void bar() {
    SESE_DEBUG("bar time up");
}

void TestTimer() {
    auto timer = sese::Timer::create(0);
    SESE_DEBUG("timer start");
    timer->delay(foo, 2);
    auto task = timer->delay(bar, 1, true);
    sese::sleep(4);
    task->cancel();
    sese::sleep(3);
    SESE_DEBUG("timer thread joining");
}

void TestTimeWheel() {
    sese::TimeWheel timer;
    timer.delay(foo, 2);
    auto task = timer.delay(bar, 1, true);
    sese::sleep(3);
    SESE_DEBUG("checking");
    timer.check();
    sese::sleep(3);
    SESE_DEBUG("checking");
    timer.check();
    timer.cancel(task);
}

TEST(TestTimer, Launcher) {
    sese::Thread th1(TestTimer, "TestTimer");
    sese::Thread th2(TestTimeWheel, "TestTimeWheel");
    th1.start();
    th2.start();
    SESE_INFO("waiting task done");
    th1.join();
    th2.join();
    SESE_INFO("all task done");
}