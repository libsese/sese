#include <sese/convert/SHA256Util.h>
#include <sese/ByteBuilder.h>
#include <sese/FileStream.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::ByteBuilder;
using sese::record::LogHelper;
using sese::ByteBuilder;
using sese::FileStream;
using sese::Test;
using sese::SHA256Util;

LogHelper helper("fSHA256"); // NOLINT

int main() {
    const char *str = "hello";
    auto src = std::make_shared<ByteBuilder>(16);
    src->write(str, 5);
    helper.info("Raw: %s", str);

    auto sha256String = SHA256Util::encode(src, false);
    assert(helper, nullptr != sha256String, -1);
    helper.info("SHA256 String: %s", &sha256String[0]);
    return 0;
}