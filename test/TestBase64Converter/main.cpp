#include "sese/util/ByteBuilder.h"
#include "sese/convert/Base64Converter.h"
#include "sese/record/LogHelper.h"
#include <cstring>
#include <codecvt>

using sese::Base64Converter;
using sese::ByteBuilder;
using sese::record::LogHelper;

sese::record::LogHelper helper("fBASE64"); // NOLINT

int main() {
    std::string str = "Hello 你好";
    char buffer[32]{0};
    helper.info("Raw: %s", str.c_str());

    auto src = std::make_shared<ByteBuilder>();
    src->write((const void *)str.c_str(), str.length());
    auto dest = std::make_shared<ByteBuilder>();
    Base64Converter::encode(src, dest);
    auto len = dest->read(buffer, 32);
    helper.info("Encoded: %s", buffer);

    src->write(buffer, len);
    memset(buffer, 0, 32);
    Base64Converter::decode(src, dest);
    dest->read(buffer, 32);
    helper.info("Decoded: %s", buffer);

    return 0;
}