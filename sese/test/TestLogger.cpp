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

#include "sese/record/Marco.h"
#include "sese/record/BlockAppender.h"
#include "sese/record/FileAppender.h"
#include "sese/record/SimpleFormatter.h"

#include "gtest/gtest.h"

#include <filesystem>

using namespace std::chrono_literals;

TEST(TestLogger, BlockAppender) {
    using sese::record::Logger;
    auto appender = std::make_shared<sese::record::BlockAppender>(1 * 1024 * 20, sese::record::Level::INFO);
    appender->size = appender->maxSize - 128;
    Logger::addGlobalLoggerAppender(appender);

    Logger::debug("no display");
    Logger::info("No.{} log message", 0);
    std::this_thread::sleep_for(100ms);

    for (auto i = 1; i < 64; i++) {
        Logger::info("No.{} log message", i);
    }

    Logger::removeGlobalLoggerAppender(appender);
}

TEST(TestLogger, FileAppender) {
    using sese::record::Logger;
    {
        auto logger = sese::record::getLogger();
        auto file_stream = sese::io::FileStream::create("hello.log", sese::io::FileStream::T_WRITE_TRUNC);
        ASSERT_TRUE(file_stream != nullptr);
        auto file_appender = std::make_shared<sese::record::FileAppender>(file_stream);
        logger->addAppender(file_appender);

        Logger::debug("Hello");
        Logger::info("Hello");
        Logger::warn("Hello");
        Logger::error("Hello");

        logger->removeAppender(file_appender);
    }
    std::filesystem::remove("hello.log");
}

TEST(TestLogger, SimpleFormat) {
    using sese::record::Logger;
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
    Logger::debug(format1.dump(event).c_str());

    auto format2 = sese::record::SimpleFormatter("li lv la");
    Logger::debug(format2.dump(event).c_str());

    auto format3 = sese::record::SimpleFormatter("fn fi fa");
    Logger::debug(format3.dump(event).c_str());

    auto format4 = sese::record::SimpleFormatter("th tn ta");
    Logger::debug(format4.dump(event).c_str());

    auto format5 = sese::record::SimpleFormatter("%m");
    Logger::debug(format5.dump(event).c_str());
}

TEST(TestLogger, Methods) {
    using sese::record::Logger;
    Logger::debug("Hello, {}", "world");
    Logger::info("Hello");
    Logger::warn("Hello");
    Logger::error("Hello");
}
