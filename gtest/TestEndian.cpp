#include "sese/util/Endian.h"
#include "gtest/gtest.h"

TEST(TestEndian, Test16) {
    int16_t i16 = 0x1234;
    int16_t ni16 = ByteSwap16(i16);
    EXPECT_TRUE(0x3412 == ni16);
}

TEST(TestEndian, Test32) {
    int32_t i32 = 0x11223344;
    int32_t ni32 = ByteSwap32(i32);
    EXPECT_TRUE(0x44332211 == ni32);
}

TEST(TestEndian, Test64) {
    uint64_t i64 = 0x1122334455667788;
    uint64_t ni64 = ByteSwap64(i64);
    EXPECT_TRUE(0x8877665544332211 == ni64);
}