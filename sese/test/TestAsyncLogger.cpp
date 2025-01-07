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

#include "sese/record/AsyncLogger.h"
#include "sese/record/BlockAppender.h"
#include "sese/record/Marco.h"
#include "sese/util/Util.h"

#include "gtest/gtest.h"

auto makeEvent(sese::record::Level level) {
    return std::make_shared<sese::record::Event>(
            sese::DateTime::now(),
            level,
            "ThreadName",
            0,
            SESE_FILENAME,
            __LINE__,
            "Hello"
    );
}

TEST(TestAsyncLogger, DISABLED_HighLoad) {
    auto logger = sese::record::AsyncLogger();
    for (auto i = 0; i < 640; i++) {
        logger.log(makeEvent(sese::record::Level::DEBUG));
    }
}

TEST(TestAsyncLogger, DISABLED_MuiltThread) {
    auto logger = sese::record::AsyncLogger();

    auto func = [&logger]() {
        for (auto i = 0; i < 640; i++) {
            logger.log(makeEvent(sese::record::Level::DEBUG));
        }
    };

    auto th1 = sese::Thread(func, "th1");
    auto th2 = sese::Thread(func, "th2");
    auto th3 = sese::Thread(func, "th3");

    th1.start();
    th2.start();
    th3.start();

    th1.join();
    th2.join();
    th3.join();
}