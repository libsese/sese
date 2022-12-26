#include <sese/InputBufferWrapper.h>
#include <sese/convert/MessageDigest.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::InputBufferWrapper;
using sese::record::LogHelper;
using sese::MessageDigest;
using sese::Test;

LogHelper helper("SHA1"); // NOLINT

int main() {
    const char *str = "abc";
    auto src = std::make_shared<InputBufferWrapper>(str, 3);
    assert(helper, nullptr != src, -1);
    helper.info("Raw: %s", str);

    auto sha1String = MessageDigest::digest(MessageDigest::SHA1, src);
    assert(helper, nullptr != sha1String, -2);
    helper.info("SHA1 String: %s", &sha1String[0]);
    return 0;
}