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

#include <sese/text/Number.h>

#include <gtest/gtest.h>

using sese::text::Number;

TEST(TestNumber, Hex) {
    {
        auto str = Number::toHex(static_cast<int64_t>(0xAABBCCDD));
        EXPECT_EQ(str, "AABBCCDD");
    }
    {
        auto str = Number::toHex(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), false);
        EXPECT_EQ(str, "ffffffffffffffff");
    }
}

TEST(TestNumber, Oct) {
    {
        auto str = Number::toOct(static_cast<int64_t>(01234567));
        EXPECT_EQ(str, "1234567");
    }
    {
        auto str = Number::toOct(static_cast<uint64_t>(0123456712345671234));
        EXPECT_EQ(str, "123456712345671234");
    }
}

TEST(TestNumber, Binary) {
    {
        auto str = Number::toBin(static_cast<uint64_t>(0b10101010101010101010101010101010));
        EXPECT_EQ(str, "10101010101010101010101010101010");
    }
    {
        auto str = Number::toBin(static_cast<int64_t>(0b1111111111111111111111111111111111111111));
        EXPECT_EQ(str, "1111111111111111111111111111111111111111");
    }
}

TEST(TestNumber, Double) {
    {
        auto str = Number::toString(3.1415926, 2);
        EXPECT_EQ(str, "3.14");
    }
    {
        auto str = Number::toString(3.1415926, 3);
        EXPECT_EQ(str, "3.141");
    }
    {
        auto str = Number::toString(-3.14, 4);
        EXPECT_EQ(str, "-3.1400");
    }
}