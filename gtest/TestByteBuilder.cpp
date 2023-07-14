#include <sese/util/ByteBuffer.h>
#include <sese/util/ByteBuilder.h>
#include <sese/record/LogHelper.h>

#include <gtest/gtest.h>

TEST(TestByteBuilder, Read) {
    auto builder = sese::ByteBuffer(16);
    ASSERT_EQ(builder.getCapacity(), 16);

    {
        // node1:
        //   "Hello World, sese"
        // node2:
        //   "\'s Byte Builder"
        const char str[] = {"Hello World, sese's Byte Builder"};
        auto len = builder.write(str, sizeof(str) - 1);
        ASSERT_EQ(len, sizeof(str) - 1);
    }
    {
        // read a part of the first node
        char str[32]{};
        auto len = builder.read(str, 10);
        ASSERT_EQ(len, 10);
        ASSERT_EQ(std::string_view(str), std::string_view("Hello Worl"));
    }
    {
        // read data between nodes
        // read latest of the first node and all the second node
        char str[32]{};
        auto len = builder.read(str, sizeof(str) - 1);
        ASSERT_EQ(len, 22);
        ASSERT_EQ(std::string_view(str), std::string_view("d, sese's Byte Builder"));
    }
    {
        // fill data
        char buffer1[1024];
        char buffer2[1024];
        memset(buffer1, 'A', 1024);
        memset(buffer2, 'B', 1024);
        auto len = builder.write(buffer1, 1024);
        ASSERT_EQ(len, 1024);
        len = builder.write(buffer2, 1024);
        ASSERT_EQ(len, 1024);
    }
    {
        // read data cross multi nodes
        char buffer[2048];
        auto len = builder.read(buffer, 4096);
        ASSERT_EQ(len, 2048);
    }

    builder.freeCapacity();
}

TEST(TestByteBuilder, Peek) {
    auto builder = sese::ByteBuffer(16);
    ASSERT_EQ(builder.getCapacity(), 16);

    {
        // node1:
        //   "Hello World, sese"
        // node2:
        //   "\'s Byte Builder"
        const char str[] = {"Hello World, sese's Byte Builder"};
        auto len = builder.write(str, sizeof(str) - 1);
        ASSERT_EQ(len, sizeof(str) - 1);
    }
    {
        // read a part of the first node
        char str[32]{};
        auto len = builder.peek(str, 10);
        ASSERT_EQ(len, 10);
        builder.trunc(len);
        ASSERT_EQ(std::string_view(str), std::string_view("Hello Worl"));
    }
    {
        // read data between nodes
        // read latest of the first node and all the second node
        char str[32]{};
        auto len = builder.peek(str, sizeof(str) - 1);
        ASSERT_EQ(len, 22);
        builder.trunc(len);
        ASSERT_EQ(std::string_view(str), std::string_view("d, sese's Byte Builder"));
    }
    {
        // fill data
        char buffer1[1024];
        char buffer2[1024];
        memset(buffer1, 'A', 1024);
        memset(buffer2, 'B', 1024);
        auto len = builder.write(buffer1, 1024);
        ASSERT_EQ(len, 1024);
        len = builder.write(buffer2, 1024);
        ASSERT_EQ(len, 1024);
    }
    {
        // read data cross multi nodes
        char buffer[2048];
        auto len = builder.peek(buffer, 4096);
        ASSERT_EQ(len, 2048);
        builder.trunc(len);
    }

    builder.freeCapacity();
}

TEST(TestByteBuilder, Misc_0) {
    auto builder = sese::ByteBuffer();
    builder.write("Hello, World", 12);

    ASSERT_EQ(builder.getCapacity(), 1024);
    ASSERT_EQ(builder.getCurrentReadPos(), 0);
    ASSERT_EQ(builder.getCurrentWritePos(), 12);
    ASSERT_EQ(builder.getLength(), 12);

    builder.trunc(12);
    ASSERT_EQ(builder.getCapacity(), 1024);
    ASSERT_EQ(builder.getCurrentReadPos(), 12);
    ASSERT_EQ(builder.getCurrentWritePos(), 12);
    ASSERT_EQ(builder.getLength(), 12);

    builder.resetPos();
    ASSERT_EQ(builder.getCapacity(), 1024);
    ASSERT_EQ(builder.getCurrentReadPos(), 0);
    ASSERT_EQ(builder.getCurrentWritePos(), 12);
    ASSERT_EQ(builder.getLength(), 12);
}

TEST(TestByteBuilder, Misc_1) {
    auto builder = sese::ByteBuilder(6);
    builder.write("Hello, World", 12);

    // deep copy
    auto i = builder;
    ASSERT_EQ(i.trunc(12), 12);
    ASSERT_EQ(i.getCurrentReadPos(), 6);
    ASSERT_EQ(builder.getCurrentReadPos(), 0);

    // move copy
    auto k = std::move(builder);
    ASSERT_EQ(k.getCurrentReadPos(), 0);
    // builder will be unusable
    ASSERT_EQ(k.trunc(12), 12);
}

TEST(TestByteBuilder, Misc_2) {
    auto builder = sese::ByteBuilder(6);
    builder.write("Hello, World", 12);
    builder.trunc(7);

    auto i = builder;
    ASSERT_EQ(i.getCurrentReadPos(), 1);
    ASSERT_EQ(i.trunc(32), 5);
}