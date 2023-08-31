#include <sese/util/InputBufferWrapper.h>
#include <sese/util/OutputBufferWrapper.h>

#include <gtest/gtest.h>

TEST(TestBufferWrapper, Operator) {
    char buffer0[] {"Hello world"};
    char buffer1[8] {};

    auto input = sese::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);
    auto output = sese::OutputBufferWrapper(buffer1, sizeof(buffer1));

    auto len = output << input;
    ASSERT_EQ(len, 8);

    output.reset();
    len = &output << input;
    ASSERT_EQ(len, 3);
}

TEST(TestBufferWrapper, Peek) {
    char buffer0[] {"Hello World"};
    auto input = sese::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);

    char buffer1[16] {};
    auto len = input.peek(buffer1, 16);
    len = input.trunc(16);
    ASSERT_EQ(len , 11);
    input.reset();
    len = input.peek(buffer1, len);
    len = input.trunc(len);
    ASSERT_EQ(len, 11);
    len = input.peek(buffer1, 0);
    len = input.trunc(0);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferWrapper, Read) {
    char buffer0[] {"Hello World"};
    auto input = sese::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);

    char buffer1[16] {};
    auto len = input.read(buffer1, 16);
    ASSERT_EQ(len , 11);
    input.reset();
    len = input.read(buffer1, len);
    ASSERT_EQ(len, 11);
    len = input.read(buffer1, 0);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferWrapper, Misc) {
    char buffer[8] {};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_EQ(output.getBuffer(), buffer);
    ASSERT_EQ(output.getCapacity(), 8);
    ASSERT_EQ(output.getLength(), 0);
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}