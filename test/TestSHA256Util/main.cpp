#include "sese/util/InputBufferWrapper.h"
#include <sese/convert/MessageDigest.h>
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"

using sese::InputBufferWrapper;
using sese::record::LogHelper;
using sese::MessageDigest;
using sese::Test;

LogHelper helper("fSHA256"); // NOLINT

int main() {
    const char *str = "11111111111111111111111111111111111111111111111111111111";
    auto src = std::make_shared<InputBufferWrapper>(str, 56);
    helper.info("Raw: %s", str);

    auto sha256String = MessageDigest::digest(MessageDigest::SHA256, src, false);
    assert(helper, nullptr != sha256String, -1);
    helper.info("SHA256 String: %s", &sha256String[0]);
    return 0;
}