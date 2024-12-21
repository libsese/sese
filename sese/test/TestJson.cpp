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
    sese::Json::streamify(&output, object.getDict());
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
    sese::Json::streamify(&output, object.getDict());
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
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_NE(object, nullptr);
    auto object = sese::Json::parse(&input);
    ASSERT_TRUE(object.isDict());

    // SESE_JSON_GET_STRING(str1, object, "str1", "undef");
    // EXPECT_EQ(str1, "\\b\\f\\t\\n\\r/\\K");
    // SESE_JSON_GET_STRING(str2, object, "str2", "undef");
    // EXPECT_EQ(str2, "Hello");
    // SESE_JSON_GET_BOOLEAN(bool1, object, "bool1", false);
    // EXPECT_EQ(bool1, true);
    // SESE_JSON_GET_BOOLEAN(bool2, object, "bool2", false);
    // EXPECT_EQ(bool2, false);
    // SESE_JSON_GET_INTEGER(int1, object, "int", 0);
    // EXPECT_EQ(int1, 1);
    // SESE_JSON_GET_DOUBLE(double1, object, "double", 0.0);
    // EXPECT_EQ(double1, 3.14);
    ASSERT_EQ(object.getDict().find("str1")->getString(), "\\b\\f\\t\\n\\r/\\K");
    ASSERT_EQ(object.getDict().find("str2")->getString(), "Hello");
    ASSERT_EQ(object.getDict().find("bool1")->getBool(), true);
    ASSERT_EQ(object.getDict().find("bool2")->getBool(), false);
    ASSERT_EQ(object.getDict().find("int")->getInt(), 1);
    ASSERT_EQ(object.getDict().find("double")->getDouble(), 3.14);

    // Test basic data that doesn't exist
    // SESE_JSON_GET_STRING(str0, object, "A", "undef");
    // EXPECT_EQ(str0, "undef");
    // SESE_JSON_GET_BOOLEAN(bool0, object, "A", true);
    // EXPECT_EQ(bool0, true);
    // SESE_JSON_GET_DOUBLE(double0, object, "A", 0.0);
    // EXPECT_EQ(double0, 0);
    // SESE_JSON_GET_INTEGER(int0, object, "A", 0);
    // EXPECT_EQ(int0, 0);

    // Test non-existent objects
    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::BasicData>("A"), nullptr);
    EXPECT_EQ(object.getDict().find("A"), nullptr);

    // Test null elements
    // SESE_JSON_GET_BOOLEAN(b, object, "nullable", false);
    // EXPECT_EQ(b, false);
    // SESE_JSON_GET_STRING(s, object, "nullable", "undef");
    // EXPECT_EQ(s, "undef");
    // SESE_JSON_GET_INTEGER(i, object, "nullable", 0);
    // EXPECT_EQ(i, 0);
    // SESE_JSON_GET_DOUBLE(d, object, "nullable", 0);
    // EXPECT_EQ(d, 0);
    EXPECT_TRUE(object.getDict().find("nullable")->isNull());

    // Test for unmatched data types
    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("str1"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("str1"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::BasicData>("object"), nullptr);
    EXPECT_FALSE(object.getDict().find("str1")->isDict());
    EXPECT_FALSE(object.getDict().find("str1")->isList());
}

// TEST(TestJson, Setter) {
//     auto object = std::make_shared<sese::json::ObjectData>();
//     SESE_JSON_SET_STRING(object, "str1", "\\b\\f\\t\\n\\r/");
//     SESE_JSON_SET_INTEGER(object, "int", 10);
//     SESE_JSON_SET_DOUBLE(object, "pi", 3.14);
//     SESE_JSON_SET_BOOLEAN(object, "t1", true);
//     SESE_JSON_SET_BOOLEAN(object, "t2", false);
//     SESE_JSON_SET_NULL(object, "nullable");

//     auto data = std::make_shared<sese::json::BasicData>();
//     data->setNull(false);
//     data->setNotNull("Hello", true);
//     object->set("str2", data);

//     // Test array macros
//     auto array = std::make_shared<sese::json::ArrayData>();
//     object->set("array", array);
//     SESE_JSON_PUT_STRING(array, "Hello");
//     SESE_JSON_PUT_STRING(array, "World");
//     SESE_JSON_PUT_BOOLEAN(array, true);
//     SESE_JSON_PUT_BOOLEAN(array, false);
//     SESE_JSON_PUT_INTEGER(array, 0);
//     SESE_JSON_PUT_DOUBLE(array, 0.0);
//     SESE_JSON_PUT_NULL(array);

//     sese::io::ConsoleOutputStream console;
//     sese::json::JsonUtil::serialize(object.get(), &console);
// }

TEST(TestJson, Value) {
    auto input = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::T_READ);
    auto object = sese::Json::parse(input.get());
    input->close();
    ASSERT_FALSE(object.isNull());

    auto content = object.toString(4);
    puts(content.c_str());
    // auto output = sese::io::FileStream::create("./2967168498.json", TEXT_WRITE_CREATE_TRUNC);
    // sese::Json::streamify(output.get(), object.getDict());
    // output->close();
}

/// Key-value pair splitting error
TEST(TestJson, Error_0) {
    const char STR[] = "{\n"
                       "  \"Hello\", \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// The object start character is incorrect
TEST(TestJson, Error_1) {
    const char STR[] = "  \"Hello\": \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// There is no next object after the comma
TEST(TestJson, Error_2) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"World\",\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// String escape is incomplete
TEST(TestJson, Error_3) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"\\";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
    auto value = sese::Json::parse(&input);
    EXPECT_TRUE(value.isNull());
}

/// tokens are empty
TEST(TestJson, Error_4) {
    const char STR[] = "";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
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
