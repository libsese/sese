#include <sese/text/StringBuffer.h>
#include <sese/text/StringBuilder.h>

#include <gtest/gtest.h>

TEST(TestStringBuilder, Construct_0) {
    auto builder0 = sese::text::StringBuffer("Hello World");
    auto builder1 = sese::text::StringBuffer(32);
    auto builder3 = sese::text::StringBuilder("Hello World");

    builder1.append("Hello World");

    auto string0 = builder0.toString();
    auto string1 = builder1.toString();
    ASSERT_EQ(string0, string1);
}

TEST(TestStringBuilder, Construct_1) {
    auto builder0 = sese::text::StringBuilder();
    builder0 << "Hello";
    auto builder1 = builder0;
    ASSERT_EQ(builder0.length(), builder1.length());

    auto builder2 = std::move(builder0);
    ASSERT_EQ(builder0.length(), 0);
    ASSERT_EQ(builder2.length(), builder1.length());
    builder2.clear();
    ASSERT_EQ(builder2.length(), 0);
}

TEST(TestStringBuilder, Reverse) {
    auto builder = sese::text::StringBuffer();
    builder << "Hello";
    builder.reverse();
    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("olleH"));
}

TEST(TestStringBuilder, Append) {
    auto builder = sese::text::StringBuffer(4);
    builder << "H";
    builder << std::string("e");
    builder << std::string_view("l");
    builder << 'l';
    builder << 'o';
    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hello"));
}

TEST(TestStringBuilder, Expansion) {
    auto builder0 = sese::text::StringBuffer(4);
    builder0 << "Hello";
    auto string0 = builder0.toString();
    ASSERT_EQ(string0, std::string_view("Hello"));

    auto builder1 = sese::text::StringBuffer(4);
    builder1 << std::string("Hello");
    auto string1 = builder1.toString();
    ASSERT_EQ(string1, std::string_view("Hello"));

    auto builder2 = sese::text::StringBuffer(4);
    builder2 << std::string_view("Hello");
    auto string2 = builder1.toString();
    ASSERT_EQ(string2, std::string_view("Hello"));
}

TEST(TestStringBuilder, Trim) {
    auto builder = sese::text::StringBuffer();
    builder.append("  Hello  ");
    builder.trim();
    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hello"));
}

TEST(TestStringBuilder, Split) {
    auto builder = sese::text::StringBuffer("Hello,World");
    auto res = builder.split(",");
    ASSERT_EQ(res[0], std::string_view("Hello"));
    ASSERT_EQ(res[1], std::string_view("World"));

    res = sese::text::StringBuilder::split("Hello,World", ",");
    ASSERT_EQ(res[0], std::string_view("Hello"));
    ASSERT_EQ(res[1], std::string_view("World"));
}

TEST(TestStringBuilder, Misc_0) {
    auto builder = sese::text::StringBuffer(16);
    builder << "Hello, World";
    ASSERT_TRUE(builder.setChatAt(0, 'h'));
    ASSERT_FALSE(builder.setChatAt(-1, 'k'));

    ASSERT_EQ(builder.getCharAt(1), 'e');
    ASSERT_EQ(builder.getCharAt(-1), 0);

    ASSERT_TRUE(builder.delCharAt(6));
    ASSERT_FALSE(builder.delCharAt(12));

    ASSERT_TRUE(builder.del(0, 5));
    ASSERT_FALSE(builder.del(12, 2));
    ASSERT_FALSE(builder.del(2, 12));

    ASSERT_TRUE(builder.insertAt(0, "Hi, "));

    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hi, World"));

    builder.insertAt(9, ", 你好");
    string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hi, World, 你好"));
}