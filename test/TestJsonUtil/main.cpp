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
    auto fileStream = FileStream::create(PROJECT_PATH "/test/TestJsonUtil/data.json", TEXT_READ_EXISTED);
    auto object = JsonUtil::deserialize(fileStream, 3);
    assert(helper, object != nullptr, -1);

    auto booleanValue = object->getDataAs<BasicData>("boolean");
    assert(helper, booleanValue != nullptr, -1);
    assert(helper, booleanValue->getDataAs<bool>(false), 0);
    booleanValue->setDataAs<bool>(false);

    auto bytes = std::make_shared<ByteBuilder>();
    JsonUtil::serialize(object, bytes);

    char buffer[1024]{};
    bytes->read(buffer, 1024);
    helper.info("%s", buffer);

    return 0;
}