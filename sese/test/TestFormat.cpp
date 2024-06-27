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
#include <sese/Log.h>

using namespace sese::text;

TEST(TestFormat, Simple) {
    int a = 1, b = 2;
    std::string c = "Hello";
    EXPECT_EQ("1, Hello, 2, World", fmt("{}, Hello, {}, World", a, b));
    EXPECT_EQ("\\{} Hello", fmt("\\{} {}", c));
    EXPECT_EQ("Hello", fmt("Hello"));
    EXPECT_EQ("World", fmt("{ Hello }", "World"));
    EXPECT_EQ("World", fmt("{ Hello \\}}", "World"));
}

TEST(TestFormat, Log) {
    SESE_DEBUG("Hello {}", 123);
    SESE_INFO("Hello {}", "World");
    SESE_WARN("Hello {}");
    SESE_ERROR("Hello \\}\\{", 3.1415);
    SESE_RAW("Hello\n", 6);
}

struct Point {
    int x;
    int y;
};

namespace sese::text::overload {
template<>
struct Formatter<Point> {
    std::string format(const Point &p) {
        return fmt("({},{})", p.x, p.y);
    }
};
} // namespace sese::text::overload

TEST(TestFormat, Formatter) {
    Point point{1, 2};
    EXPECT_EQ("(1,2)", fmt("{}", point));
}