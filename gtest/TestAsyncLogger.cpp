#include "sese/record/AsyncLogger.h"
#include "sese/record/BlockAppender.h"
#include "sese/util/Util.h"

#include "gtest/gtest.h"

auto makeEvent(sese::record::Level level) {
    return std::make_shared<sese::record::Event>(
            sese::DateTime::now(),
            level,
            "ThreadName",
            0,
            __FILE__,
            __LINE__,
            "Hello"
    );
}

TEST(TestAsyncLogger, Appender) {
    auto appender = std::make_shared<sese::record::BlockAppender>(1 * 1024 * 20, sese::record::Level::INFO);

    auto logger = sese::record::AsyncLogger();
    logger.addAppender(appender);

    logger.log(makeEvent(sese::record::Level::DEBUG));
    logger.log(makeEvent(sese::record::Level::INFO));
    logger.log(makeEvent(sese::record::Level::WARN));
    logger.log(makeEvent(sese::record::Level::ERR));
    sese::sleep(0);
}

TEST(TestAsyncLogger, HighLoad) {
    auto logger = sese::record::AsyncLogger();
    for (auto i = 0; i < 640; i++) {
        logger.log(makeEvent(sese::record::Level::DEBUG));
    }
}

TEST(TestAsyncLogger, MuiltThread) {
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