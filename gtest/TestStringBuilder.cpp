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
    builder0.clear();
    builder1.clear();
    builder3.clear();

    ASSERT_TRUE(builder0.empty());
    ASSERT_EQ(builder0.length(), 0);
    ASSERT_NE(builder0.size(), 0);
}

TEST(TestStringBuilder, Construct_1) {
    auto builder0 = sese::text::StringBuilder();
    builder0 << "Hello";
    auto builder1 = builder0;
    ASSERT_EQ(builder0.length(), builder1.length());

    auto builder2 = std::move(builder0);
    ASSERT_EQ(builder0.length(), 0); // NOLINT
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
    builder << sese::text::String((const char *) "l", 1);
    builder << sese::text::StringView("o");
    builder << '!';
    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hello!"));
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

    builder.trim();
    string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hello"));

    builder.clear();
    builder << "    ";
    builder.trim();
    string = builder.toString();
    ASSERT_EQ(string, std::string_view(""));
}

TEST(TestStringBuilder, Split) {
    auto builder = sese::text::StringBuffer("Hello,World,");
    auto res = builder.split(",");
    ASSERT_EQ(res[0], std::string_view("Hello"));
    ASSERT_EQ(res[1], std::string_view("World"));

    res = sese::text::StringBuilder::split("Hello,World,", ",");
    ASSERT_EQ(res[0], std::string_view("Hello"));
    ASSERT_EQ(res[1], std::string_view("World"));
}

TEST(TestStringBuilder, Del) {
    auto buffer = sese::text::StringBuilder();
    buffer << "Hello, World";
    buffer.del(5, 7);
    ASSERT_EQ(buffer.toString(), std::string_view("Hello"));
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

    ASSERT_TRUE(builder.del(0, 6));
    ASSERT_FALSE(builder.del(12, 2));
    ASSERT_FALSE(builder.del(2, 99));
    ASSERT_FALSE(builder.del(-1, 12));

    ASSERT_TRUE(builder.insertAt(0, " "));
    ASSERT_TRUE(builder.insertAt(0, std::string(",")));
    ASSERT_TRUE(builder.insertAt(0, std::string_view(" ")));
    ASSERT_TRUE(builder.insertAt(0, sese::text::String("i", 1)));
    ASSERT_TRUE(builder.insertAt(0, sese::text::StringView("H")));
    ASSERT_FALSE(builder.insertAt(100, ""));

    auto string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hi , World"));

    builder.insertAt(10, ", 你好");
    string = builder.toString();
    ASSERT_EQ(string, std::string_view("Hi , World, 你好"));

    auto sstring = builder.toSString();
    ASSERT_EQ(sstring, sese::text::StringView("Hi , World, 你好"));
}