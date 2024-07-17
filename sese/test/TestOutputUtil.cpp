#include "sese/io/OutputBufferWrapper.h"
#include "sese/io/OutputUtil.h"

#include <gtest/gtest.h>

TEST(TestOutputUtil, Vector) {
    char buffer[8];
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    output << std::vector<int>{1, 2};

    auto a = reinterpret_cast<int *>(&buffer[0]);
    auto b = reinterpret_cast<int *>(&buffer[4]);
    ASSERT_EQ(*a, 1);
    ASSERT_EQ(*b, 2);
}

TEST(TestOutputUtil, Array) {
    char buffer[8];
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    output << std::array<int, 2>{3, 4};
    auto a = reinterpret_cast<int *>(&buffer[0]);
    auto b = reinterpret_cast<int *>(&buffer[4]);
    ASSERT_EQ(*a, 3);
    ASSERT_EQ(*b, 4);
}

TEST(TestOutputUtil, String) {
    char buffer[2];
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    output << "ab";
    ASSERT_EQ(buffer[0], 'a');
    ASSERT_EQ(buffer[1], 'b');

    output.reset();
    output << std::string("cd");
    ASSERT_EQ(buffer[0], 'c');
    ASSERT_EQ(buffer[1], 'd');

    output.reset();
    output << std::string_view("ef");
    ASSERT_EQ(buffer[0], 'e');
    ASSERT_EQ(buffer[1], 'f');
}

TEST(TestOutputUtil, Bitset) {
    char buffer[9]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer) - 1);
    output << std::bitset<8>("11111111");
    ASSERT_EQ(std::string_view(buffer), std::string_view("11111111"));
}

#if SESE_CXX_STANDARD > 201709L

TEST(TestOutputUtil, Span) {
    char buffer[16]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer) - 1);

    const char STR[]{"Hello World"};
    std::span<const char> span(STR);
    ASSERT_EQ(output << span, sizeof(STR));
}

#endif