#include <convert/MD5Util.h>
#include <ByteBuilder.h>
#include <FileStream.h>
#include <record/LogHelper.h>
#include <Test.h>

using sese::MD5Util;
using sese::ByteBuilder;
using sese::FileStream;
using sese::LogHelper;
using sese::Test;

LogHelper helper("fMD5"); // NOLINT

int main() {
    const char *str = "hello";
    auto src = std::make_shared<ByteBuilder>(16);
    src->write(str, 5);
    helper.info("Raw: %s", str);

    auto md5String  = MD5Util::encode(src, false);
    Test::assert(helper, !md5String.empty());
    helper.info("MD5 String: %s", md5String.c_str());

    return 0;
}