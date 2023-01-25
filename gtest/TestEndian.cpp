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
    sese::record::LogHelper log("MemoryViewer");
    uint32_t value = 0x12345678;
    char buffer[128]{};
    sese::OutputBufferWrapper output0(buffer, 64);
    sese::OutputBufferWrapper output1(buffer + 64, 64);
    sese::MemoryViewer::peer32(&output0, &value, EndianType::Big);
    sese::MemoryViewer::peer32(&output1, &value, EndianType::Little);
    log.info("value view on big endian   : %s", buffer);
    log.info("value view on little endian: %s", buffer+64);
}