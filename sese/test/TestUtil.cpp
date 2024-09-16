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

TEST(TestUtil, Overflow) {
    EXPECT_TRUE(sese::isAdditionOverflow<int32_t>(INT32_MAX, 1));
    EXPECT_TRUE(sese::isAdditionOverflow<int32_t>(INT32_MAX, INT32_MAX));
    EXPECT_FALSE(sese::isAdditionOverflow<int32_t>(1, INT32_MAX - 1));

    EXPECT_TRUE(sese::isAdditionOverflow<uint32_t>(UINT32_MAX, 1));
    EXPECT_TRUE(sese::isAdditionOverflow<uint32_t>(UINT32_MAX, UINT32_MAX));
    EXPECT_FALSE(sese::isAdditionOverflow<uint32_t>(1, UINT32_MAX - 1));

    EXPECT_TRUE(sese::isSubtractionOverflow<int32_t>(-2, INT32_MAX));
    EXPECT_TRUE(sese::isSubtractionOverflow<int32_t>(INT32_MIN, INT32_MAX));
    EXPECT_FALSE(sese::isSubtractionOverflow<int32_t>(1, INT32_MAX));

    EXPECT_TRUE(sese::isSubtractionOverflow<uint32_t>(0 , 1));
    EXPECT_TRUE(sese::isSubtractionOverflow<uint32_t>(0 , INT32_MAX));
    EXPECT_FALSE(sese::isSubtractionOverflow<uint32_t>(UINT32_MAX, UINT32_MAX));

}