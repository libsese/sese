#include <sese/util/Util.h>
#include <sese/io/OutputBufferWrapper.h>
#include <sese/text/String.h>

#include <gtest/gtest.h>

TEST(TestUtil, Find) {
    const char *str = "1234567890";
    auto i = sese::findFirstAt(str, '3');
    EXPECT_EQ(i, 2);

    auto j = sese::findFirstAt(str, 'a');
    EXPECT_EQ(j, -1);
}

TEST(TestUtil, Operator) {
    auto i = sese::StrCmpI()("ABC", "abc");
    EXPECT_EQ(i, 0);

    auto j = sese::StrCmp()("ABC", "abc");
    EXPECT_NE(j, 0);
}

TEST(TestUtil, Number2StringLength) {
    EXPECT_EQ(sese::number2StringLength(-1, 10), 2);
    EXPECT_EQ(sese::number2StringLength(0, 10), 1);
    EXPECT_EQ(sese::number2StringLength(65535, 10), 5);
    EXPECT_EQ(sese::number2StringLength(-128, 10), 4);

    EXPECT_EQ(sese::number2StringLength(129, 16), 2);
    EXPECT_EQ(sese::number2StringLength(18794, 8), 5);
}

TEST(TestUtil, Misc) {
    puts(::getSpecificVersion());

    auto err = sese::getErrorCode();
    auto str = sese::getErrorString(err);
    puts(str.c_str());
}

TEST(TestUtil, ToInt) {
    EXPECT_EQ(sese::toInteger("128", 10), 128);
    EXPECT_EQ(sese::toInteger("99e", 10), 99);
}

TEST(TestUtil, Strcmp) {
    auto i = sese::strcmpDoNotCase("ABC", "abc");
    EXPECT_EQ(i, true);

    auto j = sese::strcmp("ABC", "abc");
    EXPECT_EQ(j, false);
}