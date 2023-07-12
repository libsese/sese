#include "sese/util/BufferedStream.h"
#include "sese/util/BufferedInputStream.h"
#include "sese/util/BufferedOutputStream.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/OutputBufferWrapper.h"
#include "sese/util/ByteBuilder.h"

#include "gtest/gtest.h"

TEST(TestBufferedStream, TestBufferedStream_0) {
    using sese::ByteBuilder;
    using sese::BufferedStream;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    auto buffered = BufferedStream(bytes, 16);
    ASSERT_EQ(buffered.getCap(), 16);
    buffered.write("Hello, World", 12);
    ASSERT_EQ(buffered.getLen(), 12);
    ASSERT_EQ(bytes->getLength(), 0);
    buffered.write("World", 5);
    buffered.write("HelloHelloHelloHello", 20);
    ASSERT_EQ(buffered.getLen(), 0);
    ASSERT_EQ(bytes->getLength(), 37);
}

TEST(TestBufferedStream, TestBufferedStream_1) {
    using sese::ByteBuilder;
    using sese::BufferedStream;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    bytes->write("Hello, World, Hello, World, Hello, World", 40);
    auto buffered = BufferedStream(bytes, 8);

    char buffer[1024]{};

    buffered.read(buffer, 1);
    ASSERT_EQ(buffered.getLen(), 8);
    ASSERT_EQ(buffered.getPos(), 1);
    ASSERT_EQ(buffer[0], 'H');

    buffered.read(buffer, 7);
    ASSERT_EQ(buffered.getLen(), 8);
    ASSERT_EQ(buffered.getPos(), 8);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ello, W"));

    buffered.read(buffer, 30);
    ASSERT_EQ(std::string_view(buffer), std::string_view("orld, Hello, World, Hello, Wor"));

    buffered.read(buffer, 3);
    buffer[2] = 0;
    ASSERT_EQ(std::string_view(buffer), std::string_view("ld"));

    auto bytes1 = std::make_shared<ByteBuilder>(1024);
    buffered.clear();
    buffered.reset(bytes1);
}