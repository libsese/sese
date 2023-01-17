#include "sese/record/LogHelper.h"
#include "sese/record/BlockAppender.h"
#include "sese/record/FileAppender.h"
#include "gtest/gtest.h"

TEST(TestLogger, BlockAppender) {
    sese::record::LogHelper log("BlockAppender");

    auto appender = std::make_shared<sese::record::BlockAppender>(40000);
    sese::record::getLogger()->addAppender(appender);

    for (auto i = 0; i < 640; i++) {
        log.info("No.%d log message", i);
    }
}

TEST(TestLogger, Logger) {
    sese::record::LogHelper log("Logger");

    log.debug("Hello");
    log.info("Hello");
    log.warn("Hello");
    log.error("Hello");
}

TEST(TestLogger, FileAppender) {
    sese::record::LogHelper log("FileAppender");

    auto logger = sese::record::getLogger();
    auto fileStream = sese::FileStream::create("hello.log", TEXT_WRITE_CREATE_TRUNC);
    ASSERT_TRUE(fileStream != nullptr);
    auto fileAppender = std::make_shared<sese::record::FileAppender>(fileStream);
    logger->addAppender(fileAppender);

    log.debug("Hello");
    log.info("Hello");
    log.warn("Hello");
    log.error("Hello");
}