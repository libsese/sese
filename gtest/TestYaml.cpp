#include "sese/config/yaml/YamlUtil.h"
#include "sese/util/InputBufferWrapper.h"

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

    auto float2Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(mappingObj->get("float2"));
    EXPECT_EQ(float2Obj->getDataAs<double>(0.0), 3.14e5);

    auto arrayObj = std::dynamic_pointer_cast<sese::yaml::ArrayData>(rootObj->get("sequence"));
    auto element1Obj = std::dynamic_pointer_cast<sese::yaml::BasicData>(*arrayObj->begin());
    EXPECT_EQ(element1Obj->getDataAs<std::string>("undef"), "element1");
}