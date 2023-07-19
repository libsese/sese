#include "sese/record/LogHelper.h"
#include "sese/util/MemoryViewer.h"
#include "sese/util/OutputBufferWrapper.h"
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

TEST(TestEndian, MemoryViewer) {
    sese::record::LogHelper log;
    uint32_t value = 0x12345678;
    char buffer[128]{};
    sese::OutputBufferWrapper output0(buffer, 64);
    sese::OutputBufferWrapper output1(buffer + 64, 64);
    sese::MemoryViewer::peer32(&output0, &value, EndianType::Big);
    sese::MemoryViewer::peer32(&output1, &value, EndianType::Little);
    log.info("value view on big endian   : %s", buffer);
    log.info("value view on little endian: %s", buffer + 64);
}

TEST(TestEndian, Host) {
    {
        uint16_t num1 = 0x1122;
        uint16_t num2 = FromLittleEndian16(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x2211);
#endif
    }

    {
        uint32_t num1 = 0x11223344;
        uint32_t num2 = FromLittleEndian32(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x44332211);
#endif
    }

    {
        uint64_t num1 = 0x1122334455667788;
        uint64_t num2 = FromLittleEndian64(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x8877665544332211);
#endif
    }

    {
        uint16_t num1 = 0x1122;
        uint16_t num2 = ToLittleEndian(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x2211);
#endif
    }

    {
        uint32_t num1 = 0x11223344;
        uint32_t num2 = ToLittleEndian(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x44332211);
#endif
    }

    {
        uint64_t num1 = 0x1122334455667788;
        uint64_t num2 = ToLittleEndian(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, num1);
#else
        EXPECT_EQ(num2, 0x8877665544332211);
#endif
    }

    {
        uint16_t num1 = 0x1122;
        uint16_t num2 = ToBigEndian16(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x2211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }

    {
        uint32_t num1 = 0x11223344;
        uint32_t num2 = ToBigEndian32(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x44332211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }

    {
        uint64_t num1 = 0x1122334455667788;
        uint64_t num2 = ToBigEndian64(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x8877665544332211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }

    {
        uint16_t num1 = 0x1122;
        uint16_t num2 = FromBigEndian16(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x2211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }

    {
        uint32_t num1 = 0x11223344;
        uint32_t num2 = FromBigEndian32(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x44332211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }

    {
        uint64_t num1 = 0x1122334455667788;
        uint64_t num2 = FromBigEndian64(num1);
#ifdef SESE_LITTLE_ENDIAN
        EXPECT_EQ(num2, 0x8877665544332211);
#else
        EXPECT_EQ(num2, num1);
#endif
    }
}