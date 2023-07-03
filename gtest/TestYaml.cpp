#include "sese/config/yaml/YamlUtil.h"
#include "sese/config/yaml/Marco.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/ConsoleOutputStream.h"

#include "gtest/gtest.h"

const char str1[]{
        "root:\n"
        "  mapping:\n"
        "    str 1: \"Hello:-|str1\"\n"
        "    \"str\'2\": 'Hello str2'\n"
        "    str\"3: \"Hello\\\"str3\"\n"
        "    str|4: 'Hello:-|\"str4'\n"
        "    str5: Hello str5\n"
        "    bool1: true\n"
        "    bool2: yes\n"
        "    bool3: or\n"
        "    int1: 114514\n"
        "    float1: 3.14\n"
        "    float2: 3.14e+5\n"
        "    null1: ~\n"
        "    null2: null\n"
        "    null3: Null\n"
        "    null4:\n"
        "\n"
        "  sequence:\n"
        "    - element1\n"
        "    - element2\n"
        "    -\n"
        "    - ~\n"
        "    - mapping1:\n"
        "      bool1: false\n"
        "      bool2: no\n"
        "    - mapping2:\n"
        "      bool1: False\n"
        "      bool2: No\n"
        "sub:\n"
        "  str5: 'Hello str5'"};

TEST(TestYaml, Tokenizer_0) {
    auto input = sese::InputBufferWrapper(str1, sizeof(str1) - 1);

    while (true) {
        decltype(auto) tuple = sese::yaml::YamlUtil::getLine(&input);
        if (std::get<1>(tuple).length() == 0) {
            break;
        }
        auto tokens = sese::yaml::YamlUtil::tokenizer(std::get<1>(tuple));
        printf("%d [", (int) std::get<0>(tuple));
        bool first = true;
        for (decltype(auto) token: tokens) {
            if (first) {
                first = false;
            } else {
                printf(", ");
            }
            printf("\"%s\"", token.c_str());
        }
        printf("]\n");
    }
}

TEST(TestYaml, Deserialize_0) {
    auto input = sese::InputBufferWrapper(str1, sizeof(str1) - 1);
    auto object = sese::yaml::YamlUtil::deserialize(&input, 5);
    ASSERT_NE(object, nullptr);
    auto obj = std::dynamic_pointer_cast<sese::yaml::ObjectData>(object);

    auto subObj = std::dynamic_pointer_cast<sese::yaml::ObjectData>(obj->get("sub"));
    auto str5Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(subObj->get("str5"));
    EXPECT_EQ(str5Obj->getDataAs<std::string>("undef"), "Hello str5");

    auto rootObj = std::dynamic_pointer_cast<sese::yaml::ObjectData>(obj->get("root"));
    auto mappingObj = std::dynamic_pointer_cast<sese::yaml::ObjectData>(rootObj->get("mapping"));

    auto str2Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("str'2"));
    EXPECT_EQ(str2Obj->getDataAs<std::string>("undef"), "Hello str2");

    auto bool2Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("bool2"));
    EXPECT_EQ(bool2Obj->getDataAs<bool>(false), true);

    auto bool3Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("bool3"));
    EXPECT_EQ(bool3Obj->getDataAs<bool>(true), false);

    auto float2Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("float2"));
    EXPECT_EQ(float2Obj->getDataAs<double>(0.0), 3.14e5);

    auto int2Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("int1"));
    EXPECT_EQ(int2Obj->getDataAs<int64_t>(0), 114514);

    auto arrayObj = std::dynamic_pointer_cast<sese::yaml::ArrayData>(rootObj->get("sequence"));
    auto element1Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(*arrayObj->begin());
    EXPECT_EQ(element1Obj->getDataAs<std::string>("undef"), "element1");

    /// 数据为 null
    SESE_YAML_GET_STRING(s0, mappingObj, "null1", "undef");
    EXPECT_EQ(s0, "undef");
    SESE_YAML_GET_BOOLEAN(b0, mappingObj, "null1", false);
    EXPECT_EQ(b0, false);
    SESE_YAML_GET_DOUBLE(d0, mappingObj, "null1", 0.0);
    EXPECT_EQ(d0, 0.0);
    SESE_YAML_GET_INTEGER(i0, mappingObj, "null1", 0);
    EXPECT_EQ(i0, 0);

    /// 数据不存在
    SESE_YAML_GET_STRING(s1, mappingObj, "A", "undef");
    EXPECT_EQ(s1, "undef");
    SESE_YAML_GET_BOOLEAN(b1, mappingObj, "A", false);
    EXPECT_EQ(b1, false);
    SESE_YAML_GET_DOUBLE(d1, mappingObj, "A", 0.0);
    EXPECT_EQ(d1, 0.0);
    SESE_YAML_GET_INTEGER(i1, mappingObj, "A", 0);
    EXPECT_EQ(i1, 0);
}

/// 字符串转义
TEST(TestYaml, Deserialize_1) {
    const char str[]{R"(str: "\n\r\'\"\a")"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto object = sese::yaml::YamlUtil::deserialize(&input, 5);
    ASSERT_NE(object, nullptr);
}

TEST(TestYaml, Serialize_0) {
    sese::ConsoleOutputStream output;

    auto root = std::make_shared<sese::yaml::ObjectData>();

    auto object = std::make_shared<sese::yaml::ObjectData>();
    root->set("object", object);
    // auto name = std::make_shared<sese::yaml::BasicData>();
    // name->setDataAs<std::string>("sese-core");
    // object->set("name", name);
    // auto version = std::make_shared<sese::yaml::BasicData>();
    // version->setDataAs<std::string>("0.6.3");
    // object->set("version", version);
    SESE_YAML_SET_STRING(object, "name", "sese-core");
    SESE_YAML_SET_STRING(object, "version", "0.6.3");
    SESE_YAML_SET_BOOLEAN(object, "bool1", false);
    SESE_YAML_SET_BOOLEAN(object, "bool2", true);
    SESE_YAML_SET_DOUBLE(object, "double", 3.14);
    SESE_YAML_SET_NULL(object, "nullable");

    auto array = std::make_shared<sese::yaml::ArrayData>();
    root->set("array", array);
    // auto element0 = std::make_shared<sese::yaml::BasicData>();
    // element0->setDataAs<int64_t>(1919810);
    // array->push(element0);
    // auto element1 = std::make_shared<sese::yaml::BasicData>();
    // element1->setDataAs<double>(3.1415926);
    // array->push(element1);
    SESE_YAML_PUT_INTEGER(array, 1919810);
    SESE_YAML_PUT_DOUBLE(array, 3.1415926);

    auto complex = std::make_shared<sese::yaml::ObjectData>();
    array->push(complex);
    // auto real = std::make_shared<sese::yaml::BasicData>();
    // real->setDataAs<int64_t>(1);
    // complex->set("real", real);
    // auto image = std::make_shared<sese::yaml::BasicData>();
    // image->setDataAs<int64_t>(-3);
    // complex->set("image", image);
    SESE_YAML_SET_INTEGER(complex, "real", 1);
    SESE_YAML_SET_INTEGER(complex, "image", -3);

    sese::yaml::YamlUtil::serialize(root, &output);
}

TEST(TestYaml, Serialize_1) {
    sese::ConsoleOutputStream output;

    auto array = std::make_shared<sese::yaml::ArrayData>();
    SESE_YAML_PUT_STRING(array, "value0");
    SESE_YAML_PUT_BOOLEAN(array, true);
    SESE_YAML_PUT_BOOLEAN(array, false);
    SESE_YAML_PUT_INTEGER(array, 114514);
    SESE_YAML_PUT_INTEGER(array, 1919810);
    SESE_YAML_PUT_DOUBLE(array, 3.14);
    SESE_YAML_PUT_DOUBLE(array, 0.15926e-2);
    SESE_YAML_PUT_NULL(array);

    sese::yaml::YamlUtil::serialize(array, &output);
}

TEST(TestYaml, Serialize_2) {
    const char str[]{"- Hello\n"
                     "- World\n"
                     "- null\n"
                     "- ~"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto object = sese::yaml::YamlUtil::deserialize(&input, 5);
    ASSERT_NE(object, nullptr);
}