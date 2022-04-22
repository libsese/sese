#include "sese/ByteBuilder.h"
#include "sese/convert/PercentConverter.h"
#include "sese/record/LogHelper.h"
#include <cstring>

using sese::ByteBuilder;
using sese::LogHelper;
using sese::PercentConverter;

LogHelper helper("fPERCENT");// NOLINT

int main() {
    char buffer[1024]{0};
    const char *string = "Percent Encoding Test | 百分号编码测试";
    auto dest = std::make_shared<ByteBuilder>();
    helper.info("Raw: %s", string);

    PercentConverter::encode(string, dest);
    dest->read(buffer, 1024);
    helper.info("Encode: %s", buffer);

    PercentConverter::decode(buffer, dest);
    memset(buffer, 0, 1024);
    dest->read(buffer, 1024);
    helper.info("Decode: %s", buffer);

    return 0;
}