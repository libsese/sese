#include <sese/util/StopWatch.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

#include <thread>

using namespace std::chrono_literals;

TEST(TestStopWatch, Construct) {
    sese::StopWatch stop_watch;
    std::this_thread::sleep_for(100ms);
    auto span = stop_watch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}

TEST(TestStopWatch, Stop) {
    sese::StopWatch stop_watch;
    stop_watch.stop();
    std::this_thread::sleep_for(100ms);
    auto span = stop_watch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}

TEST(TestStopWatch, Reset) {
    sese::StopWatch stop_watch;
    std::this_thread::sleep_for(100ms);
    stop_watch.reset();
    std::this_thread::sleep_for(100ms);
    auto span = stop_watch.stop();
    SESE_INFO("cost time %f ms", span.getTotalMilliseconds());
}