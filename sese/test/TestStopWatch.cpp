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

#define SESE_C_LIKE_FORMAT

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