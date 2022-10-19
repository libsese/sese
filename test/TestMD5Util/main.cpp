#include <sese/convert/MD5Util.h>
#include <sese/ByteBuilder.h>
#include <sese/FileStream.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::record::LogHelper;
using sese::ByteBuilder;
using sese::FileStream;
using sese::Test;
using sese::Test;
using sese::MD5Util;

LogHelper helper("fMD5"); // NOLINT

int main() {
    const char *str = "hello";
    auto src = std::make_shared<ByteBuilder>(16);
    src->write(str, 5);
    helper.info("Raw: %s", str);

    auto md5String  = MD5Util::encode(src, false);
    assert(helper, nullptr != md5String, -1);
    helper.info("MD5 String: %s", &md5String[0]);

    return 0;
}