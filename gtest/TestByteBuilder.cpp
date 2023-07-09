#include <sese/util/ByteBuilder.h>
#include <sese/record/LogHelper.h>

#include <gtest/gtest.h>

TEST(TestByteBuilder, Read) {
    auto builder = sese::ByteBuilder(16);

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
        // read latest of the first node and all of the second node
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
        auto len = builder.read(buffer, 2048);
        ASSERT_EQ(len, 2048);
    }
}
