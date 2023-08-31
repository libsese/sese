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

TEST(TestTimer, Normal) {
    auto timer = sese::Timer::create(0);
    sese::record::LogHelper::d("timer start");
    timer->delay(foo, 2);
    auto task = timer->delay(bar, 1, true);
    sese::sleep(4);
    task->cancel();
    sese::sleep(3);
    sese::record::LogHelper::d("timer thread joining");
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}