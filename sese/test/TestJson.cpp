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
#include <sese/config/Json.h>
#include <sese/io/ConsoleOutputStream.h>
#include <sese/io/FileStream.h>
#include <sese/io/InputBufferWrapper.h>

/// Parse the JSON format from the file
TEST(TestJson, FromFile) {
    auto file_stream = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::T_READ);

    auto object = sese::Json::parse(file_stream.get());
    ASSERT_TRUE(object.isDict());

    EXPECT_EQ(object.getDict().find("A"), nullptr);

    EXPECT_TRUE(object.getDict().find("object")->isDict());
    EXPECT_TRUE(object.getDict().find("mixin_array")->isList());
    EXPECT_FALSE(object.getDict().find("value")->isNull());

    EXPECT_TRUE(object.getDict().find("boolean")->isBool());
    EXPECT_TRUE(object.getDict().find("boolean")->getBool());

    auto output = sese::io::ConsoleOutputStream();
    sese::Json::streamify(&output, object);
    output.write("\n", 1);
}

TEST(TestJson, FromFileSimd) {
    auto file_stream = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::B_READ);

    auto object = sese::Json::simdParse(file_stream.get());
    ASSERT_TRUE(object.isDict());

    EXPECT_EQ(object.getDict().find("A"), nullptr);

    EXPECT_TRUE(object.getDict().find("object")->isDict());
    EXPECT_TRUE(object.getDict().find("mixin_array")->isList());
    EXPECT_FALSE(object.getDict().find("value")->isNull());

    EXPECT_TRUE(object.getDict().find("boolean")->isBool());
    EXPECT_TRUE(object.getDict().find("boolean")->getBool());

    auto output = sese::io::ConsoleOutputStream();
    sese::Json::streamify(&output, object);
    output.write("\n", 1);
}

TEST(TestJson, Getter) {
    const char STR[] = "{\n"
                       "  \"str1\": \"\\b\\f\\t\\n\\r/\\K\",\n"
                       "  \"str2\": \"Hello\",\n"
                       "  \"bool1\": true,\n"
                       "  \"bool2\": false,\n"
                       "  \"int\": 1,\n"
                       "  \"double\": 3.14\n"
                       "  \"nullable\": null\n"
                       "  \"object\": {} \n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto object = sese::Json::parse(&input);
    ASSERT_TRUE(object.isDict());

    ASSERT_EQ(object.getDict().find("str1")->getString(), "\\b\\f\\t\\n\\r/\\K");
    ASSERT_EQ(object.getDict().find("str2")->getString(), "Hello");
    ASSERT_EQ(object.getDict().find("bool1")->getBool(), true);
    ASSERT_EQ(object.getDict().find("bool2")->getBool(), false);
    ASSERT_EQ(object.getDict().find("int")->getInt(), 1);
    ASSERT_EQ(object.getDict().find("double")->getDouble(), 3.14);

    // Test non-existent objects
    EXPECT_EQ(object.getDict().find("A"), nullptr);

    // Test null elements
    EXPECT_TRUE(object.getDict().find("nullable")->isNull());

    // Test for unmatched data types
    EXPECT_FALSE(object.getDict().find("str1")->isDict());
    EXPECT_FALSE(object.getDict().find("str1")->isList());
}

TEST(TestJson, Value) {
    auto input = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::T_READ);
    auto object = sese::Json::parse(input.get());
    input->close();
    ASSERT_FALSE(object.isNull());

    auto content = object.toString(4);
    puts(content.c_str());
}

/// Key-value pair splitting error
TEST(TestJson, Error_0) {
    const char STR[] = "{\n"
                       "  \"Hello\", \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// The object start character is incorrect
TEST(TestJson, Error_1) {
    const char STR[] = "  \"Hello\": \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// There is no next object after the comma
TEST(TestJson, Error_2) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"World\",\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// String escape is incomplete
TEST(TestJson, Error_3) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"\\";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// tokens are empty
TEST(TestJson, Error_4) {
    const char STR[] = "";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// Subarray error
TEST(TestJson, Error_5) {
    const char STR[] = "{\n"
                       "  \"str1\": [\n"
                       "    \"Hello\": \"World\"\n"
                       "  ]\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// The child object is incorrect
TEST(TestJson, Error_6) {
    const char STR[] = "{\n"
                       "  \"str1\": {\n"
                       "    \"Hello\": ,\n"
                       "    \"World\": 1\n"
                       "  }\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// Incomplete objects
TEST(TestJson, Error_7) {
    const char STR[] = "{\n"
                       "  \"str1\": {\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// Incomplete arrays
TEST(TestJson, Error_8) {
    const char STR[] = "{\n"
                       "  \"str1\": [\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}
