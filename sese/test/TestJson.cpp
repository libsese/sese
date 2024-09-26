#include <gtest/gtest.h>
#include <sese/config/Json.h>
#include <sese/io/ConsoleOutputStream.h>
#include <sese/io/FileStream.h>
#include <sese/io/InputBufferWrapper.h>

/// 从文件解析 Json 格式
TEST(TestJson, FromFile) {
    auto file_stream = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::T_READ);
    // auto object = sese::json::JsonUtil::deserialize(file_stream, 3);
    // ASSERT_TRUE(object != nullptr);

    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::BasicData>("A"), nullptr);

    // EXPECT_NE(object->getDataAs<sese::json::ObjectData>("object"), nullptr);
    // EXPECT_NE(object->getDataAs<sese::json::ArrayData>("mixin_array"), nullptr);
    // EXPECT_NE(object->getDataAs<sese::json::BasicData>("value"), nullptr);

    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("mixin_array"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("object"), nullptr);

    // auto boolean_value = object->getDataAs<sese::json::BasicData>("boolean");
    // ASSERT_TRUE(boolean_value != nullptr);
    // ASSERT_TRUE(boolean_value->getDataAs<bool>(false));
    // boolean_value->setDataAs<bool>(false);

    // auto output = std::make_shared<sese::io::ConsoleOutputStream>();
    // sese::json::JsonUtil::serialize(object, output);
    // output->write("\n", 1);
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

    // 测试不存在的基本数据
    // SESE_JSON_GET_STRING(str0, object, "A", "undef");
    // EXPECT_EQ(str0, "undef");
    // SESE_JSON_GET_BOOLEAN(bool0, object, "A", true);
    // EXPECT_EQ(bool0, true);
    // SESE_JSON_GET_DOUBLE(double0, object, "A", 0.0);
    // EXPECT_EQ(double0, 0);
    // SESE_JSON_GET_INTEGER(int0, object, "A", 0);
    // EXPECT_EQ(int0, 0);

    // 测试不存在的对象
    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("A"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::BasicData>("A"), nullptr);

    // 测试 null 元素
    // SESE_JSON_GET_BOOLEAN(b, object, "nullable", false);
    // EXPECT_EQ(b, false);
    // SESE_JSON_GET_STRING(s, object, "nullable", "undef");
    // EXPECT_EQ(s, "undef");
    // SESE_JSON_GET_INTEGER(i, object, "nullable", 0);
    // EXPECT_EQ(i, 0);
    // SESE_JSON_GET_DOUBLE(d, object, "nullable", 0);
    // EXPECT_EQ(d, 0);

    // 测试不匹配的数据类型
    // EXPECT_EQ(object->getDataAs<sese::json::ObjectData>("str1"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::ArrayData>("str1"), nullptr);
    // EXPECT_EQ(object->getDataAs<sese::json::BasicData>("object"), nullptr);
}

TEST(TestJson, Setter) {
    // auto object = std::make_shared<sese::json::ObjectData>();
    // SESE_JSON_SET_STRING(object, "str1", "\\b\\f\\t\\n\\r/");
    // SESE_JSON_SET_INTEGER(object, "int", 10);
    // SESE_JSON_SET_DOUBLE(object, "pi", 3.14);
    // SESE_JSON_SET_BOOLEAN(object, "t1", true);
    // SESE_JSON_SET_BOOLEAN(object, "t2", false);
    // SESE_JSON_SET_NULL(object, "nullable");

    // auto data = std::make_shared<sese::json::BasicData>();
    // data->setNull(false);
    // data->setNotNull("Hello", true);
    // object->set("str2", data);

    // // 测试数组宏
    // auto array = std::make_shared<sese::json::ArrayData>();
    // object->set("array", array);
    // SESE_JSON_PUT_STRING(array, "Hello");
    // SESE_JSON_PUT_STRING(array, "World");
    // SESE_JSON_PUT_BOOLEAN(array, true);
    // SESE_JSON_PUT_BOOLEAN(array, false);
    // SESE_JSON_PUT_INTEGER(array, 0);
    // SESE_JSON_PUT_DOUBLE(array, 0.0);
    // SESE_JSON_PUT_NULL(array);

    // sese::io::ConsoleOutputStream console;
    // sese::json::JsonUtil::serialize(object.get(), &console);
}


TEST(TestJson, Value) {
    auto input = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.json", sese::io::FileStream::T_READ);
    auto object = sese::Json::parse(input.get(), 4);
    input->close();
    ASSERT_FALSE(object.isNull());

    auto content = object.toString(4);
    puts(content.c_str());
    // auto output = sese::io::FileStream::create("./2967168498.json", TEXT_WRITE_CREATE_TRUNC);
    // sese::Json::streamify(output.get(), object.getDict());
    // output->close();
}

/// 键值对分割错误
TEST(TestJson, Error_0) {
    const char STR[] = "{\n"
                       "  \"Hello\", \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 对象起始符错误
TEST(TestJson, Error_1) {
    const char STR[] = "  \"Hello\": \"World\"\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 逗号后不存在下一个对象
TEST(TestJson, Error_2) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"World\",\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 字符串转义不完整
TEST(TestJson, Error_3) {
    const char STR[] = "{\n"
                       "  \"Hello\": \"\\";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// tokens 为空
TEST(TestJson, Error_4) {
    const char STR[] = "";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 子数组错误
TEST(TestJson, Error_5) {
    const char STR[] = "{\n"
                       "  \"str1\": [\n"
                       "    \"Hello\": \"World\"\n"
                       "  ]\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 子对象错误
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
}

/// 不完整对象
TEST(TestJson, Error_7) {
    const char STR[] = "{\n"
                       "  \"str1\": {\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 不完整数组
TEST(TestJson, Error_8) {
    const char STR[] = "{\n"
                       "  \"str1\": [\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 5);
    // ASSERT_EQ(object, nullptr);
}

/// 构造对象时，子数组深度超过限制
TEST(TestJson, Error_9) {
    const char STR[] = "{\n"
                       "  \"arr\": []\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 1);
    // ASSERT_EQ(object, nullptr);
}

/// 构造对象时，子对象深度超过限制
TEST(TestJson, Error_10) {
    const char STR[] = "{\n"
                       "  \"obj\": {}\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 1);
    // ASSERT_EQ(object, nullptr);
}

/// 构造数组时，子数组深度超过限制
TEST(TestJson, Error_11) {
    const char STR[] = "{\n"
                       "  \"arr\": [[]]\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 2);
    // ASSERT_EQ(object, nullptr);
}

/// 构造数组时，子对象深度超过限制
TEST(TestJson, Error_12) {
    const char STR[] = "{\n"
                       "  \"arr\": [{}]\n"
                       "}";
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    // auto object = sese::json::JsonUtil::deserialize(&input, 2);
    // ASSERT_EQ(object, nullptr);
}