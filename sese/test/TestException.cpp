#define SESE_C_LIKE_FORMAT

#include <sese/record/Marco.h>
#include <sese/util/Exception.h>
#include <sese/io/ConsoleOutputStream.h>

#include <gtest/gtest.h>

void event() {
    throw sese::Exception("my exception");
}

void handle() {
    event();
}

TEST(TestException, PrintStacktrace) {
    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                e.printStacktrace();
                throw;
            } catch (...) {
                throw;
            },
            sese::Exception
    );

    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                sese::io::ConsoleOutputStream console;
                e.printStacktrace(&console);
                throw;
            },
            sese::Exception
    );

    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                SESE_EXCEPT(e);
                throw;
            },
            sese::Exception
    );
}