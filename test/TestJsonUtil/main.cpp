#include <sese/config/json/JsonUtil.h>
#include <sese/FileStream.h>
#include <sese/ConsoleOutputStream.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::ConsoleOutputStream;
using sese::FileStream;
using sese::Test;
using sese::json::ArrayData;
using sese::json::BasicData;
using sese::json::JsonUtil;
using sese::json::ObjectData;
using sese::record::LogHelper;

sese::record::LogHelper helper("fJSON");// NOLINT

int main() {
    auto fileStream = FileStream::create(PROJECT_PATH "/test/TestJsonUtil/data.json", TEXT_READ_EXISTED);
    auto object = JsonUtil::deserialize(fileStream, 3);
    assert(helper, object != nullptr, -1);

    auto booleanValue = object->getDataAs<BasicData>("boolean");
    assert(helper, booleanValue != nullptr, -1);
    assert(helper, booleanValue->getDataAs<bool>(false), 0);
    booleanValue->setDataAs<bool>(false);

    auto output = std::make_shared<ConsoleOutputStream>();
    JsonUtil::serialize(object, output);
    output->write("\n", 1);

    return 0;
}