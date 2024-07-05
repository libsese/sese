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

using namespace sese::text;

using MyMap = std::map<std::string, int>;
using MyMapPair = std::pair<const std::string, int>;

struct Point {
    int x;
    int y;
};

namespace sese::text::overload {
template<>
struct Formatter<Point> {
    static bool parse(const std::string &args) {
        SESE_INFO("args {}", args);
        return true;
    }
    static std::string format(const Point &p) {
        return fmt("({},{})", p.x, p.y);
    }
};

template<>
struct Formatter<MyMapPair> {
    void parse(const std::string &) {}
    static std::string format(MyMapPair &pair) {
        return "{" + pair.first + "," + std::to_string(pair.second) + "}";
    }
};
} // namespace sese::text::overload

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
}

TEST(TestFormat, Formatter) {
    Point point{1, 2};
    SESE_INFO(R"(\{{\}123\}ABC\}})", point);
    auto datetime = sese::DateTime::now();
    SESE_INFO("{} | {HH:mm:ss}", datetime, datetime);
    SESE_INFO("{A}", "Hello");
}

TEST(TestFormat, MismatchParam) {
    SESE_INFO("{}");
    SESE_INFO("{}",1, 2);
}

TEST(TestFormat, Constexpr) {
    constexpr auto PATTERN = "{} {}";
    EXPECT_EQ(2, FormatParameterCounter(PATTERN));
}

TEST(TestFormat, Iterable) {
    auto array = std::array<int, 3>({1, 2, 3});
    EXPECT_EQ("[1,2,3]", fmt("{}", array));

    MyMap map;
    map["abc"] = 114;
    map["efg"] = 514;
    EXPECT_EQ("[{abc,114},{efg,514}]", fmt("{}", map));

    SESE_INFO("{A,B}|{C,D}", map, array);
}