#include <sese/convert/SHA1Util.h>
#include <sese/FileStream.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>
#include <sese/InputBufferWrapper.h>

using sese::record::LogHelper;
using sese::FileStream;
using sese::Test;
using sese::SHA1Util;
using sese::InputBufferWrapper;

LogHelper helper("SHA1"); // NOLINT

int main() {
    const char *str = "abc";
    auto src = std::make_shared<InputBufferWrapper>(str, 3);
    assert(helper, nullptr != src, -1);
    helper.info("Raw: %s", str);

    auto sha1String = SHA1Util::encode(src, false);
    assert(helper, nullptr != sha1String, -2);
    helper.info("SHA1 String: %s", &sha1String[0]);
    return 0;
}