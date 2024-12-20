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

#include "sese/record/Marco.h"
#include "sese/record/LogHelper.h"
#include "sese/record/BlockAppender.h"
#include "sese/record/FileAppender.h"
#include "sese/record/SimpleFormatter.h"

#include "gtest/gtest.h"

#include <filesystem>

using namespace std::chrono_literals;

TEST(TestLogger, BlockAppender) {
    sese::record::LogHelper log;

    auto appender = std::make_shared<sese::record::BlockAppender>(1 * 1024 * 20, sese::record::Level::INFO);
    appender->size = appender->maxSize - 128;
    sese::record::Logger::addGlobalLoggerAppender(appender);

    log.debug("no display");
    log.info("No.%d log message", 0);
    std::this_thread::sleep_for(100ms);

    for (auto i = 1; i < 64; i++) {
        log.info("No.%d log message", i);
    }

    sese::record::Logger::removeGlobalLoggerAppender(appender);
}

TEST(TestLogger, Logger) {
    sese::record::LogHelper log;

    log.debug("Hello");
    log.info("Hello");
    log.warn("Hello");
    log.error("Hello");
}

TEST(TestLogger, StaticMethod) {
    using sese::record::LogHelper;
    LogHelper::d("Hello");
    LogHelper::i("Hello");
    LogHelper::w("Hello");
    LogHelper::e("Hello");
}

TEST(TestLogger, FileAppender) {
    {
        sese::record::LogHelper log;

        auto logger = sese::record::getLogger();
        auto file_stream = sese::io::FileStream::create("hello.log", sese::io::FileStream::T_WRITE_TRUNC);
        ASSERT_TRUE(file_stream != nullptr);
        auto file_appender = std::make_shared<sese::record::FileAppender>(file_stream);
        logger->addAppender(file_appender);

        log.debug("Hello");
        log.info("Hello");
        log.warn("Hello");
        log.error("Hello");

        logger->removeAppender(file_appender);
    }
    std::filesystem::remove("hello.log");
}

TEST(TestLogger, SimpleFormat) {
    auto event = std::make_shared<sese::record::Event>(
            sese::DateTime::now(),
            sese::record::Level::INFO,
            "ThreadName",
            0,
            SESE_FILENAME,
            __LINE__,
            "Hello"
    );


    auto format1 = sese::record::SimpleFormatter("c");
    sese::record::LogHelper::d(format1.dump(event).c_str());

    auto format2 = sese::record::SimpleFormatter("li lv la");
    sese::record::LogHelper::d(format2.dump(event).c_str());

    auto format3 = sese::record::SimpleFormatter("fn fi fa");
    sese::record::LogHelper::d(format3.dump(event).c_str());

    auto format4 = sese::record::SimpleFormatter("th tn ta");
    sese::record::LogHelper::d(format4.dump(event).c_str());

    auto format5 = sese::record::SimpleFormatter("%m");
    sese::record::LogHelper::d(format5.dump(event).c_str());
}

TEST(TestLogger, Macro) {
    SESE_DEBUG("Hello");
    SESE_INFO("Hello");
    SESE_WARN("Hello");
    SESE_ERROR("Hello");
    SESE_RAW("Hello\n", 6);
}
