#include "sese/util/FileStream.h"
#include "sese/util/ConsoleOutputStream.h"
#include "sese/config/json/JsonUtil.h"
#include "gtest/gtest.h"

TEST(TestJson, Json) {
    auto fileStream = sese::FileStream::create(PROJECT_PATH "/test/TestJsonUtil/data.json", TEXT_READ_EXISTED);
    auto object = sese::json::JsonUtil::deserialize(fileStream, 3);
    ASSERT_TRUE(object != nullptr);

    auto booleanValue = object->getDataAs<sese::json::BasicData>("boolean");
    ASSERT_TRUE(booleanValue != nullptr);
    ASSERT_TRUE(booleanValue->getDataAs<bool>(false));
    booleanValue->setDataAs<bool>(false);

    auto output = std::make_shared<sese::ConsoleOutputStream>();
    sese::json::JsonUtil::serialize(object, output);
    output->write("\n", 1);
}