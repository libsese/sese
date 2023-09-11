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
    ASSERT_EQ(len, data0.length());

    std::string data1 = "ABCDEFGHIJK";
    // len = buffer.write(data1.c_str(), data1.length());
    len = buffer << data1;
    ASSERT_EQ(len, size - data0.length());
}

TEST(TestFixedBuffer, Read) {
    auto size = 16;
    auto buffer = sese::io::FixedBuffer(16);
    ASSERT_EQ(buffer.write("ABCDEFGHIJKLMNOP", 16), 16);

    int64_t len;
    char buf[9];
    len = buffer.read(buf, 9);
    ASSERT_EQ(len, 9);

    len = buffer.read(buf, 9);
    ASSERT_EQ(len, 7);
}

TEST(TestFixedBuffer, Peek) {
    auto size = 16;
    auto buffer = sese::io::FixedBuffer(16);
    ASSERT_EQ(buffer << "ABCDEFGHIJKLMNOP", 16);

    int64_t len;
    char buf[9];
    len = buffer.peek(buf, 9);
    ASSERT_EQ(len, 9);
    buffer.trunc(len);

    len = buffer.peek(buf, 9);
    ASSERT_EQ(len, 7);
    ASSERT_EQ(buffer.trunc(9), 7);
}

TEST(TestFixedBuffer, Misc_0) {
    auto buffer = sese::io::FixedBuilder(10);
    buffer.write("Hello", 5);

    auto i = buffer;
    ASSERT_EQ(i.getWriteableSize(), buffer.getWriteableSize());

    auto k = std::move(buffer);
    ASSERT_EQ(buffer.getWriteableSize(), 0);
    k.reset();
    ASSERT_EQ(k.getWriteableSize(), 10);
}

TEST(TestFixedBuffer, Misc_1) {
    auto buffer = sese::io::FixedBuffer(10);
    buffer.write("Hello", 5);
    ASSERT_EQ(buffer.getReadableSize(), 5);
    ASSERT_EQ(buffer.getWriteableSize(), 5);
    buffer.reset();
    ASSERT_EQ(buffer.getReadableSize(), 0);
    ASSERT_EQ(buffer.getWriteableSize(), 10);
}