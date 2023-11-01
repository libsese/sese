#include <sese/text/Number.h>

#include <gtest/gtest.h>

using sese::text::Number;

TEST(TestNumber, Hex) {
    {
        auto str = Number::toHex(static_cast<int64_t>(0xAABBCCDD));
        EXPECT_EQ(str, "AABBCCDD");
    }
    {
        auto str = Number::toHex(0xFFFFFFFFFFFFFFFF, false);
        EXPECT_EQ(str, "ffffffffffffffff");
    }
}

TEST(TestNumber, Oct) {
    {
        auto str = Number::toOct(static_cast<int64_t>(01234567));
        EXPECT_EQ(str, "1234567");
    }
    {
        auto str = Number::toOct(static_cast<uint64_t>(0123456712345671234));
        EXPECT_EQ(str, "123456712345671234");
    }
}

TEST(TestNumber, Binary) {
    {
        auto str = Number::toBin(static_cast<uint64_t>(0b10101010101010101010101010101010));
        EXPECT_EQ(str, "10101010101010101010101010101010");
    }
    {
        auto str = Number::toBin(static_cast<int64_t>(0b1111111111111111111111111111111111111111));
        EXPECT_EQ(str, "1111111111111111111111111111111111111111");
    }
}