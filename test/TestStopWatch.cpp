#include <sese/util/StopWatch.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

#include <thread>

using namespace std::chrono_literals;

TEST(TestStopWatch, Construct) {
    sese::StopWatch stopWatch;
    std::this_thread::sleep_for(100ms);
    auto span = stopWatch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}

TEST(TestStopWatch, Stop) {
    sese::StopWatch stopWatch;
    stopWatch.stop();
    std::this_thread::sleep_for(100ms);
    auto span = stopWatch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}

TEST(TestStopWatch, Reset) {
    sese::StopWatch stopWatch;
    std::this_thread::sleep_for(100ms);
    stopWatch.reset();
    std::this_thread::sleep_for(100ms);
    auto span = stopWatch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}