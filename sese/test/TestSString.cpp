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

#include "sese/text/SString.h"
#include <sese/text/Algorithm.h>
#include <sese/text/String.h>

#include <gtest/gtest.h>

const char *str0 = "Hello 你好 こんにちは";
const char *sub0 = "你好";
const char *sub1 = "He";
const char *sub2 = "にちは";

TEST(TestSStringAlgol, Normal) {
    EXPECT_TRUE(sstr::NORMAL(str0, sub0) == 6);
    EXPECT_TRUE(sstr::NORMAL(str0, sub1) == 0);
    EXPECT_TRUE(sstr::NORMAL(str0, sub2) == 19);
}

TEST(TestSStringAlgol, BM) {
    EXPECT_TRUE(sstr::BM(str0, sub0) == 6);
    EXPECT_TRUE(sstr::BM(str0, sub1) == 0);
    EXPECT_TRUE(sstr::BM(str0, sub2) == 19);
}

TEST(TestSStringAlgol, KMP) {
    EXPECT_TRUE(sstr::KMP(str0, sub0) == 6);
    EXPECT_TRUE(sstr::KMP(str0, sub1) == 0);
    EXPECT_TRUE(sstr::KMP(str0, sub2) == 19);
}

TEST(TestSString, Parse_0) {
    const char *str = "你好 こんにちは Hello";
    auto string = sese::text::String::fromUTF8(str);

    // printf("string.data = %s\n", string.data());
    // printf("string.len = %d\n", (int) string.len());
    // printf("string.cap = %d\n", (int) string.cap());
    // printf("string.size = %d\n", (int) string.size());
    // printf("string.empty = %s\n", string.empty() ? "true" : "false");

    EXPECT_EQ(string.len(), 14);
    EXPECT_EQ(string.size(), 28);
    EXPECT_EQ(string.cap(), 32);
    EXPECT_FALSE(string.empty());
}

TEST(TestSString, Parse_1) {
    const char *str = "你好 こんにちは Hello";
    auto string = sese::text::String::fromUTF8(str);

    const char *s1 = "你こH";
    auto ch0 = sstr::getUnicodeFromUTF8Char(s1);
    // printf("ch0 = %u\n", (uint32_t) ch0);

    auto str1 = sese::text::String::fromUTF8(s1);

    // printf("str1.data = %s\n", str1.data());
    // printf("string.at(0) == str1.at(0) = %s\n", string.at(0) == str1.at(0) ? "true" : "false");
    // printf("string.at(3) == str1.at(1) = %s\n", string.at(3) == str1.at(1) ? "true" : "false");
    // printf("string[9] == str1[2] = %s\n", string[9] == str1[2] ? "true" : "false");

    EXPECT_EQ(ch0.code, 20320);
    EXPECT_EQ(string.at(0), str1.at(0));
    EXPECT_EQ(string.at(3), str1.at(1));
    EXPECT_EQ(string[9], str1[2]);
}

TEST(TestSString, Iterator) {
    const char *str = "你好 こんにちは Hello";
    auto string = sese::text::String::fromUTF8(str);
    const char *s1 = "你こH";
    auto str1 = sese::text::String::fromUTF8(s1);

    auto chars = string.toChars();
    for (auto &&ch: chars) {
        printf("\\u%04X ", ch.code);
    }
    puts("");

    for (auto &&ch: string) {
        printf("\\u%04X ", ch.code);
    }
    puts("");

    EXPECT_EQ(sese::text::String::fromSChars(chars), "你好 こんにちは Hello");
}

TEST(TestSStringView, Parse_0) {
    const char *str = "你好 こんにちは Hello";
    auto string1 = sese::text::String::fromUTF8(str);
    auto string2 = sese::text::String::fromUTF8("你こH");
    auto string3 = sese::text::StringView(str);
    // printf("string1.data = %s\n", string3.data());
    // printf("string == string1 = %s\n", string1 == string3 ? "true" : "false");
    // printf("string != str = %s\n", string1 != str ? "true" : "false");
    // printf("string == str1 = %s\n", string1 == string2 ? "true" : "false");
    // puts("");

    EXPECT_EQ(string1, string3);
    EXPECT_EQ(string1, str);
    EXPECT_NE(string1, string2);
}

TEST(TestSStringView, Append) {
    auto front = sese::text::StringView("你好");
    auto back = sese::text::StringView("Hello");
    const char *extra = "こんにちは";
    auto res0 = front + back;
    // printf("front + back = %s\n", res0.data());
    EXPECT_EQ(res0, "你好Hello");
    res0 += extra;
    // printf("front + back + extra = %s\n", res0.data());
    EXPECT_EQ(res0, "你好Helloこんにちは");
}

TEST(TestSStringView, Misc) {
    auto string = sese::text::String::fromUTF8("你好 こんにちは Hello");
    auto pos0 = string.findByBytes("こん");
    auto pos1 = string.find("にちは");
    EXPECT_EQ(pos0, 7);
    EXPECT_EQ(pos1, 5);

    auto space_string = sese::text::StringView("  こんにちは  ");
    auto trim_string = space_string.trim();
    // printf("after trim: %s\n", trimString.data());
    // printf("after reverse: %s\n", trimString.reverse().data());
    // puts("");
    EXPECT_EQ(trim_string, "こんにちは");

    auto spilt_string = sese::text::StringView("こんにちは、わたくしはSStringです");
    printf("spilt string: %s\n", spilt_string.data());
    auto spilt_res = spilt_string.split("は");
    for (auto i: spilt_res) {
        printf("%s\n", i.data());
    }
}

TEST(TestSString, WString) {
    auto string = sese::text::String::fromUTF8("你好 こんにちは Hello");
    auto wstring = sese::text::String::fromUCS2LE(L"你好 こんにちは Hello");
    EXPECT_EQ(wstring, string);
    auto wstr = string.toCWString();
    EXPECT_EQ(wcscmp(wstr.get(), L"你好 こんにちは Hello"), 0);

    auto substr0 = wstring.substring(3);
    auto substr1 = wstring.substring(3, 5);
    auto substr2 = wstring.substring(9, 10);
    // printf("sub0 = %s\n", substr0.data());
    // printf("sub1 = %s\n", substr1.data());
    // printf("sub2 = %s\n", substr2.data());

    EXPECT_EQ(substr0, "こんにちは Hello");
    EXPECT_EQ(substr1, "こんにちは");
    EXPECT_EQ(substr2, "Hello");
}

TEST(TestSString, Misc) {
    auto str = sese::text::StringView();
    // printf("str.null = %s\n", str.null() ? "true" : "false");
    EXPECT_TRUE(str.null());

    str = sese::text::StringView("你吃了吗？");
    auto ends_word = sese::text::StringView("吗？");
    auto ends_fail_word = sese::text::StringView("吗?");
    auto long_end_word = sese::text::StringView("今晚你吃了吗？");
    // printf("str.endsWith = %s\n", str.endsWith(endsWord) ? "true" : "false");
    EXPECT_TRUE(str.endsWith(ends_word));
    EXPECT_FALSE(str.endsWith(long_end_word));
    EXPECT_FALSE(str.endsWith(ends_fail_word));
    EXPECT_TRUE(str.endsWith("了吗？"));
    EXPECT_FALSE(str.endsWith("今晚你吃了吗？"));
    EXPECT_FALSE(str.endsWith("吃了吗?"));

    str = sese::text::StringView("你好 hello");
    // printf("str.isLower = %s\n", str.isLower() ? "true" : "false");
    // printf("str.isUpper = %s\n", str.isUpper() ? "true" : "false");
    EXPECT_TRUE(str.isLower());
    EXPECT_FALSE(str.isUpper());

    auto upper = str.toUpper();
    auto lower = str.toLower();
    // printf("str.toUpper = %s\n", upper.data());
    // printf("str.toLower = %s\n", lower.data());
    EXPECT_EQ(upper, "你好 HELLO");
    EXPECT_EQ(lower, "你好 hello");

    lower.toUpper();
    // printf("lower.toUpper = %s\n", lower.data());
    EXPECT_EQ(lower, "你好 HELLO");
    upper.toLower();
    // printf("upper.toLower = %s\n", upper.data());
    EXPECT_EQ(upper, "你好 hello");

    auto old_str = sese::text::String::fromUTF8("hello");
    auto chars = old_str.toChars();
    auto new_str = sese::text::String::fromSChars(chars.data(), chars.size());
    new_str.toUpper();
    EXPECT_TRUE(old_str.isLower());
    EXPECT_FALSE(new_str.isLower());
}