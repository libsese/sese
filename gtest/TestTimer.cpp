#include <sese/record/LogHelper.h>
#include <sese/util/Timer.h>
#include <sese/util/Util.h>

#include <gtest/gtest.h>

void foo() {
    sese::record::LogHelper::d("foo time up");
}

void bar() {
    sese::record::LogHelper::d("bar time up");
}

TEST(TestTimer, _0) {
    auto timer = sese::Timer::create(5);
    sese::record::LogHelper::d("timer start");
    timer->delay(foo, 2);
    auto task = timer->delay(bar, 1, true);
    sese::sleep(4);
    task->cancel();
    sese::sleep(3);
    sese::record::LogHelper::d("timer thread joining");
    timer->shutdown();
}