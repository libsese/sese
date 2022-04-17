#include "ByteBuilder.h"
#include "convert/Base64Converter.h"
#include "record/LogHelper.h"
#include <cstring>

using sese::Base64Converter;
using sese::ByteBuilder;
using sese::LogHelper;

LogHelper helper("fBASE64"); // NOLINT

int main() {
    std::string str = "Hello";
    char buffer[16]{0};
    helper.info("Raw: %s", str.c_str());

    auto src = std::make_shared<ByteBuilder>();
    src->write((const void *)str.c_str(), str.length());
    auto dest = std::make_shared<ByteBuilder>();
    Base64Converter::encode(src, dest);
    auto len = dest->read(buffer, 16);
    helper.info("Encoded: %s", buffer);

    src->write(buffer, len);
    memset(buffer, 0, 16);
    Base64Converter::decode(src, dest);
    dest->read(buffer, 16);
    helper.info("Decoded: %s", buffer);

    return 0;
}