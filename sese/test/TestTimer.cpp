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

#include <sese/log/Marco.h>
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
    timer.delay(foo, 10);
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