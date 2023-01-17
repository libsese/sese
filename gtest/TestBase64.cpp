#include "sese/record/LogHelper.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/ConsoleOutputStream.h"
#include "sese/convert/Base64Converter.h"
#include "gtest/gtest.h"

TEST(TestBase64, Base64) {
    std::string str = "你好";
    auto input0 = sese::InputBufferWrapper(str.c_str(), str.length());
    sese::ConsoleOutputStream output;
    // 5L2g5aW9
    sese::Base64Converter::encode(&input0, &output, sese::Base64Converter::CodePage::BASE64);
    puts("");

    std::string code = "5L2g5aW9";
    auto input1 = sese::InputBufferWrapper(code.c_str(), code.length());
    sese::Base64Converter::decode(&input1, &output);
    puts("");
}

TEST(TestBase64, Base62) {
    std::string str = "你好";
    auto input0 = sese::InputBufferWrapper(str.c_str(), str.length());
    sese::ConsoleOutputStream output;
    // 5L2g5aW9
    sese::Base64Converter::encode(&input0, &output, sese::Base64Converter::CodePage::BASE62);
    puts("");

    std::string code = "5L2g5aW9";
    auto input1 = sese::InputBufferWrapper(code.c_str(), code.length());
    sese::Base64Converter::decode(&input1, &output);
    puts("");
}