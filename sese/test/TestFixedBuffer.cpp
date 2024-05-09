#include "sese/io/OutputUtil.h"
#include "sese/io/FixedBuffer.h"
#include "sese/io/FixedBuilder.h"

#include <gtest/gtest.h>

TEST(TestFixedBuffer, Write) {
    auto size = 16;
    auto buffer = sese::io::FixedBuffer(size);

    int64_t len;
    std::string data0 = "1234567890";
    // len = buffer.write(data0.data(), data0.length());
    len = buffer << data0;
    EXPECT_EQ(len, data0.length());

    std::string data1 = "ABCDEFGHIJK";
    // len = buffer.write(data1.c_str(), data1.length());
    len = buffer << data1;
    EXPECT_EQ(len, size - data0.length());
}

TEST(TestFixedBuffer, Read) {
    auto buffer = sese::io::FixedBuffer(16);
    EXPECT_EQ(buffer.write("ABCDEFGHIJKLMNOP", 16), 16);

    int64_t len;
    char buf[9];
    len = buffer.read(buf, 9);
    EXPECT_EQ(len, 9);

    len = buffer.read(buf, 9);
    EXPECT_EQ(len, 7);
}

TEST(TestFixedBuffer, Peek) {
    auto buffer = sese::io::FixedBuffer(16);
    EXPECT_EQ(buffer << "ABCDEFGHIJKLMNOP", 16);

    int64_t len;
    char buf[9];
    len = buffer.peek(buf, 9);
    EXPECT_EQ(len, 9);
    buffer.trunc(len);

    len = buffer.peek(buf, 9);
    EXPECT_EQ(len, 7);
    EXPECT_EQ(buffer.trunc(9), 7);
}

TEST(TestFixedBuffer, Misc_0) {
    auto buffer = sese::io::FixedBuilder(10);
    buffer.write("Hello", 5);

    auto i = buffer;
    EXPECT_EQ(i.getWriteableSize(), buffer.getWriteableSize());

    auto k = std::move(buffer);
    EXPECT_EQ(buffer.getWriteableSize(), 0);
    k.reset();
    EXPECT_EQ(k.getWriteableSize(), 10);
}

TEST(TestFixedBuffer, Misc_1) {
    auto buffer = sese::io::FixedBuffer(10);
    ASSERT_NE(buffer.data(), nullptr);
    buffer.write("Hello", 5);
    EXPECT_EQ(buffer.getSize(), 10);
    EXPECT_EQ(buffer.getReadableSize(), 5);
    EXPECT_EQ(buffer.getWriteableSize(), 5);
    buffer.reset();
    EXPECT_EQ(buffer.getReadableSize(), 0);
    EXPECT_EQ(buffer.getWriteableSize(), 10);
}