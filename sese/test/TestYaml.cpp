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

#include "sese/util/Value.h"
#include <cstdint>
#include <cstdio>
#include <sese/io/InputBufferWrapper.h>
#include <sese/io/ConsoleOutputStream.h>
#include <sese/config/Yaml.h>

#include <gtest/gtest.h>

constexpr char STR1[]{R"(
root:
  mapping:
    str 1: "Hello:-|str1"
    str\'2: 'Hello str2'
    str\"3: "Hello\\\"str3"
    str|4: 'Hello:-|\"str4'
    str5: Hello str5
    bool1: true
    bool2: yes
    bool3: or
    int1: 114514
    float1: 3.14
    float2: 3.14e+5
    null1: ~
    null2: null
    null3: Null
    null4:
  sequence:
    - element1
    - element2
    -
    - ~
    - mapping1:
      bool1: false
      bool2: no
    - mapping2:
      bool1: False
      bool2: No
    - mapping3: value
      ip: 0.0.0.0
      port: 8080
    - - hello
      - world
sub:
  str5: 'Hello str5'
)"};

TEST(TestYaml, Deserialize_0) {
    auto input = sese::io::InputBufferWrapper(STR1, sizeof(STR1) - 1);
    auto object = sese::Yaml::parse(&input);
    ASSERT_TRUE(object.isDict());
    auto &obj_dict = object.getDict();

    {
        auto sub_obj = obj_dict.find("sub");
        ASSERT_TRUE(sub_obj->isDict());
        auto &sub_obj_dict = sub_obj->getDict();
        auto str5_obj = sub_obj_dict.find("str5");
        ASSERT_TRUE(str5_obj->isString());
        EXPECT_EQ(str5_obj->getString(), "Hello str5");
    }

    {
        auto root_obj = obj_dict.find("root");
        ASSERT_TRUE(root_obj->isDict());
        auto &root_obj_dict = root_obj->getDict();
        auto mapping_obj = root_obj_dict.find("mapping");
        ASSERT_TRUE(mapping_obj->isDict());
        auto &mapping_obj_dict = mapping_obj->getDict();

        {
            auto str2_obj = mapping_obj_dict.find("str\\'2");
            ASSERT_TRUE(str2_obj->isString());
            EXPECT_EQ(str2_obj->getString(), "Hello str2");

            auto bool2_obj = mapping_obj_dict.find("bool2");
            ASSERT_TRUE(bool2_obj->isBool());
            EXPECT_EQ(bool2_obj->getBool(), true);

            auto bool3_obj = mapping_obj_dict.find("bool3");
            ASSERT_FALSE(bool3_obj->isBool());

            auto float2_obj = mapping_obj_dict.find("float2");
            ASSERT_TRUE(float2_obj->isDouble());
            EXPECT_EQ(float2_obj->getDouble(), 3.14e5);

            auto int2_obj = mapping_obj_dict.find("int1");
            ASSERT_TRUE(int2_obj->isInt());
            EXPECT_EQ(int2_obj->getInt(), 114514);
        }

        auto array_obj = root_obj_dict.find("sequence");
        ASSERT_TRUE(array_obj->isList());
        auto element1_obj = *array_obj->getList().begin();
        ASSERT_TRUE(element1_obj->isString());
        EXPECT_EQ(element1_obj->getString(), "element1");

        auto element7_obj = *(array_obj->getList().end() - 2);
        ASSERT_TRUE(element7_obj->isDict());
        auto ip = element7_obj->getDict().find("ip");
        ASSERT_NE(ip, nullptr);
        EXPECT_EQ(ip->getString(), "0.0.0.0");
        auto port = element7_obj->getDict().find("port");
        ASSERT_NE(port, nullptr);
        EXPECT_EQ(port->getInt(), 8080);

        auto element8_obj = *(array_obj->getList().end() - 1);
        ASSERT_TRUE(element8_obj->isList());

        auto s0 = mapping_obj_dict.find("null1");
        EXPECT_TRUE(s0->isNull());
        auto b0 = mapping_obj_dict.find("null1");
        EXPECT_TRUE(b0->isNull());
        auto d0 = mapping_obj_dict.find("null1");
        EXPECT_TRUE(d0->isNull());
        auto i0 = mapping_obj_dict.find("null1");
        EXPECT_TRUE(i0->isNull());

        auto s1 = mapping_obj_dict.find("A");
        EXPECT_EQ(s1, nullptr);
    }
}

/// String escape
TEST(TestYaml, Deserialize_1) {
    constexpr char STR[]{R"(str: "\n\r\'\"\a")"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

/// createObject
/// The next line does not exist and the current element is null
TEST(TestYaml, Deserialize_2) {
    constexpr char STR[]{"root:\n"
                         "  null: "};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

/// createObject
/// The current row element is null,
/// and the next element is still the object to which it belongs
TEST(TestYaml, Deserialize_3) {
    constexpr char STR[]{"root:\n"
                         "  a:\n"
                         "  b: str"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

/// createArray
/// If the next line does not exist, skip the object
TEST(TestYaml, Deserialize_4) {
    constexpr char STR[]{"root:\n"
                         "  - ele:"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

TEST(TestYaml, Deserialize_5) {
    constexpr char STR[]{"- Hello\n"
                         "- World\n"
                         "- null\n"
                         "- ~"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

/// The object has no child elements
TEST(TestYaml, Deserialize_6) {
    constexpr char STR[]{"root:"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_FALSE(OBJECT.isNull());
}

TEST(TestYaml, Serialize_0) {
    using sese::Value;
    sese::io::ConsoleOutputStream output;

    // clang-format off
    auto root = Value(Value::Dict()
        .set("name", "sese-core")
        .set("version", "0.6.3")
        .set("bool1", false)
        .set("bool2", true)
        .set("double", 3.14)
        .set("nullable", "~")
        .set("array", Value::List()
            .append(INT64_C(1919810))
            .append(3.1415926)
            .append(true)
            .append("string")
            .append(Value::List()
                .append(INT64_C(1919810))
                .append(3.1415926)
                .append(true)
                .append("string")
            )
            .append(Value::Dict()
                .set("Hello", "World")
            )
        )
        .set("image", Value::Dict()
            .set("real", INT64_C(1))
            .set("imag", INT64_C(-3))
        )
    );
    // clang-format on

    sese::Yaml::streamify(&output, root);
}

TEST(TestYaml, Serialize_1) {
    using sese::Value;
    sese::io::ConsoleOutputStream output;

    // clang-format off
    auto root = Value(Value::List()
        .append("value0")
        .append(true)
        .append(false)
        .append(INT64_C(114514))
        .append(INT64_C(1919810))
        .append(3.14)
        .append(0.15926e-2)
        .append(Value{})
    );
    // clang-format on
    sese::Yaml::streamify(&output, root);
}

TEST(TestYaml, Serialize_2) {
    sese::io::ConsoleOutputStream output;
    sese::Value data("data");
    sese::Yaml::streamify(&output, data);
}

TEST(TestYaml, Value) {
    auto input = sese::io::InputBufferWrapper(STR1, sizeof(STR1) - 1);
    const auto VALUE = sese::Yaml::parse(&input);
    ASSERT_FALSE(VALUE.isNull());
    const auto CONTENT = VALUE.toString(5);
    puts(CONTENT.c_str());
}

TEST(TestYaml, Error_1) {
    constexpr char STR[]{"root1:\n"
                         ":root2:"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    const auto OBJECT = sese::Yaml::parse(&input);
    ASSERT_TRUE(OBJECT.isNull());
}