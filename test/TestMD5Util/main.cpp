#include <sese/InputBufferWrapper.h>
#include <sese/convert/MessageDigest.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::InputBufferWrapper;
using sese::record::LogHelper;
using sese::MessageDigest;
using sese::Test;

LogHelper helper("fMD5"); // NOLINT

int main() {
    const char *str = "hello";
    auto src = std::make_shared<InputBufferWrapper>(str, 5);
    helper.info("Raw: %s", str);

    auto md5String  = MessageDigest::digest(MessageDigest::MD5, src);
    assert(helper, nullptr != md5String, -1);
    helper.info("MD5 String: %s", &md5String[0]);

    return 0;
}