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

TEST(TestAsyncLogger, Appender) {
    // auto appender = std::make_shared<sese::record::BlockAppender>(1 * 1024 * 20, sese::record::Level::INFO);

    auto logger = sese::record::AsyncLogger();
    // logger.addAppender(appender);

    __SESE_DEBUG((&logger), "Hello");
    __SESE_INFO((&logger), "Hello");
    __SESE_WARN((&logger), "Hello");
    __SESE_ERROR((&logger), "Hello");

    // sese::sleep(0);
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