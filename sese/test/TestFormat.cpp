// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <sese/text/Format.h>
#include <sese/util/DateTime.h>
#include <sese/Log.h>

#include <map>
#include <limits>

using namespace sese::text;

struct Point {
    int x;
    int y;
};

template<>
struct sese::text::overload::Formatter<Point> {
    static bool parse(const std::string &args) {
        SESE_INFO("args {}", args);
        return true;
    }
    static void format(FmtCtx &ctx, const Point &p) {
        ctx.builder << fmt("({},{})", p.x, p.y);
    }
}; // namespace sese::text::overload

TEST(TestFormat, FmtCtx) {
    {
        // Minimal format
        std::string args;
        FmtCtx ctx("{}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("", args);
    }
    {
        // Parameter escape character 1
        std::string args;
        FmtCtx ctx("\\{\\} {}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("", args);
        EXPECT_EQ("{} ", ctx.builder.toString());
    }
    {
        // Parameter escape character 2
        std::string args;
        FmtCtx ctx("\\}\\{ {}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("", args);
        EXPECT_EQ("}{ ", ctx.builder.toString());
    }
    {
        // Parameter escape character 3
        std::string args;
        FmtCtx ctx("ABC\\}EDF\\{ {}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("", args);
        EXPECT_EQ("ABC}EDF{ ", ctx.builder.toString());
    }
    {
        // Escape characters within parameters 1
        std::string args;
        FmtCtx ctx("{\\{,\\}}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("{,}", args);
    }
    {
        // Escape characters within parameters 2
        std::string args;
        FmtCtx ctx("{ABC\\},EFG\\{}");
        EXPECT_TRUE(ctx.parsing(args));
        EXPECT_EQ("ABC},EFG{", args);
    }
}

TEST(TestFormat, OptionParse) {
    {
        FormatOption option{};
        EXPECT_TRUE(option.parse(":^3.2f"));
        EXPECT_EQ(option.align, Align::CENTER);
        EXPECT_EQ(option.wide_char, ' ');
        EXPECT_EQ(option.wide, 3);
        EXPECT_EQ(option.float_placeholder, 2);
        EXPECT_EQ(option.ext_type, 'f');
    }
    {
        FormatOption option{};
        EXPECT_TRUE(option.parse(":<.2%"));
        EXPECT_EQ(option.align, Align::LEFT);
        EXPECT_EQ(option.wide_char, ' ');
        EXPECT_EQ(option.wide, 0);
        EXPECT_EQ(option.float_placeholder, 2);
        EXPECT_EQ(option.ext_type, '%');
    }
    {
        FormatOption option{};
        EXPECT_TRUE(option.parse(":x>"));
        EXPECT_EQ(option.align, Align::RIGHT);
        EXPECT_EQ(option.wide_char, 'x');
        EXPECT_EQ(option.wide, 0);
        EXPECT_EQ(option.float_placeholder, 0);
        EXPECT_EQ(option.ext_type, '\0');
    }

    {
        FormatOption option{};
        EXPECT_TRUE(option.parse(":d"));
        EXPECT_EQ(option.ext_type, 'd');
    }
    {
        FormatOption option{};
        EXPECT_TRUE(option.parse(":"));
    }
    {
        FormatOption option{};
        EXPECT_FALSE(option.parse("0"));
    }
    {
        FormatOption option{};
        EXPECT_FALSE(option.parse(":d0"));
    }
}

TEST(TestFormat, Align) {
    EXPECT_EQ("Hello ", fmt("{:^6}", "Hello"));
    EXPECT_EQ(" Hello ", fmt("{:^7}", "Hello"));
    EXPECT_EQ("Hello ", fmt("{:<6}", "Hello"));
    EXPECT_EQ(" Hello", fmt("{:>6}", "Hello"));

    EXPECT_EQ("HelloA", fmt("{:A^6}", "Hello"));
    EXPECT_EQ("AHelloA", fmt("{:A^7}", "Hello"));
    EXPECT_EQ("AHello", fmt("{:A>6}", "Hello"));
    EXPECT_EQ("HelloA", fmt("{:A<6}", "Hello"));
}

TEST(TestFormat, Number) {
    EXPECT_EQ("   114514", fmt("{:>9}", 114514));
    EXPECT_EQ("      514", fmt("{:>9}", 514));
    EXPECT_EQ("  14.000%", fmt("{:>9.3%}", 0.14));
    EXPECT_EQ("      3.1", fmt("{:>9}", 3.14));
    EXPECT_EQ("0x001E240", fmt("0x{:0>7X}", 123456));
    EXPECT_EQ("0x001e240", fmt("0x{:0>7x}", 123456));
    EXPECT_EQ("000030071", fmt("0{:0>8o}", 12345));
    EXPECT_EQ("b01111011", fmt("b{:0>8b}", 123));

    EXPECT_EQ("123 ", fmt("{:<4}", 123));
    EXPECT_EQ(" 123 ", fmt("{:^5}", 123));
}

TEST(TestFormat, Float) {
    EXPECT_EQ("12.30%", fmt("{:<1.2%}", 0.123));
    EXPECT_EQ("12.30% ", fmt("{:<7.2%}", 0.123));
    EXPECT_EQ(" 12.30% ", fmt("{:^8.2%}", 0.123));
    EXPECT_EQ(" 12.30%", fmt("{:>7.2%}", 0.123));
    EXPECT_EQ("NaN", fmt("{}", std::numeric_limits<double>::quiet_NaN()));
}

TEST(TestFormat, Formatter) {
    // Point point{1, 2};
    // SESE_INFO(R"(\{{\}123\}ABC\}})", point);
    // auto datetime = sese::DateTime::now();
    // SESE_INFO("{} | {HH:mm:ss}", datetime, datetime);
    // SESE_INFO("{A}", "Hello");
    EXPECT_EQ("buffer[1] = 0x1e240" ,fmt("buffer[{}] = 0x{:x}", 1, 123456));
}

TEST(TestFormat, MismatchParam) {
    std::string str = "!{Mismatch in number of parameters}";
    EXPECT_EQ(str, fmt("{}"));
    EXPECT_EQ(str, fmt("{}", 1, 2));
}

TEST(TestFormat, Constexpr) {
    constexpr auto PATTERN = "{} {}";
    EXPECT_EQ(2, FormatParameterCounter(PATTERN));
}

TEST(TestFormat, Iterable) {
    auto array = std::array<int, 3>({1, 2, 3});
    EXPECT_EQ("[1,2,3]", fmt("{}", array));
    EXPECT_EQ("<1:2:3>", fmt("{<:>}", array));
    EXPECT_EQ("{1,2,3}", fmt("{\\{,\\}}", array));
    // EXPECT_EQ("!{parsing failed}", fmt("{\\{:\\}}", array));
}

TEST(TestFormat, ContainerForEach) {
    std::map<std::string, int> map{{"Hello", 10}, {"World", 2}};
    EXPECT_EQ("[{Hello, 10}, {World, 2}]", fmt("{}", for_each(map)));
    map.clear();
    EXPECT_EQ("[]", fmt("{}", for_each(map)));

    auto array = std::vector<int>({1, 2, 3});
    EXPECT_EQ("[1, 2, 3]", fmt("{}", for_each(array)));
    array.clear();
    EXPECT_EQ("[]", fmt("{}", for_each(array)));
}