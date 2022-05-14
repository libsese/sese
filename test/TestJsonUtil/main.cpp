#include <sese/config/json/JsonUtil.h>
#include <sese/FileStream.h>
#include <sese/ByteBuilder.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::ByteBuilder;
using sese::FileStream;
using sese::LogHelper;
using sese::Test;
using sese::json::JsonUtil;
using sese::json::ArrayData;
using sese::json::BasicData;
using sese::json::ObjectData;

LogHelper helper("fJSON"); // NOLINT

int main() {
    auto fileStream = std::make_shared<FileStream>();
    fileStream->open(PROJECT_PATH "/test/TestJsonUtil/data.json", TEXT_READ_EXISTED);
    auto object = JsonUtil::deserialize(fileStream, 3);
    Test::assert(helper, object != nullptr, -1);

    auto booleanValue = object->getDataAs<BasicData>("boolean");
    Test::assert(helper, booleanValue != nullptr, -1);
    Test::assert(helper, booleanValue->getDataAs<bool>(false));
    booleanValue->setDataAs<bool>(false);

    auto bytes = std::make_shared<ByteBuilder>();
    JsonUtil::serialize(object, bytes);

    char buffer[1024]{};
    bytes->read(buffer, 1024);
    helper.info("%s", buffer);

    return 0;
}