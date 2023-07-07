#include "sese/util/InputBufferWrapper.h"
#include "sese/util/OutputBufferWrapper.h"
#include "sese/convert/Base64Converter.h"
#include "gtest/gtest.h"

TEST(TestBase64, Base64Encode_0) {
    char buffer[32]{};
    std::string str = "你好";
    auto input = sese::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("5L2g5aW9"));
}

TEST(TestBase64, Base64Encode_1) {
    char buffer[32]{};
    std::string str = "sese-core-";
    auto input = sese::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("c2VzZS1jb3JlLQ=="));
}

TEST(TestBase64, Base64Decode_0) {
    char buffer[32]{};
    std::string code = "5L2g5aW9";
    auto input = sese::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(std::string_view("你好"), std::string_view(buffer));
}

TEST(TestBase64, Base64Decode_1) {
    char buffer[32]{};
    std::string code = "c2VzZS1jb3JlLQ==";
    auto input = sese::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(std::string_view("sese-core-"), std::string_view(buffer));
}

TEST(TestBase64, EncodeInteger) {
    char buffer[32]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encodeInteger(7562611, &output, sese::Base64Converter::Base62CodePage);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ftXl"));
}

TEST(TestBase64, DecodeBuffer) {
    const char *code = "ftXl";
    auto num = sese::Base64Converter::decodeBuffer((const unsigned char *)code, 4, sese::Base64Converter::Base62CodePage);
    ASSERT_EQ(num, 7562611);
}

//TEST(TestBase62, Base62Encode_0) {
//    char buffer[32]{};
//    std::string str = "你好";
//    auto input = sese::InputBufferWrapper(str.c_str(), str.length());
//    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
//    sese::Base64Converter::encode(&input, &output, sese::Base64Converter::CodePage::BASE62);
//    ASSERT_EQ(std::string_view(buffer), std::string_view("5L2g5aW9"));
//}
//
//TEST(TestBase62, Base62Encode_1) {
//    char buffer[32]{};
//    std::string str = "sese-core-library";
//    auto input = sese::InputBufferWrapper(str.c_str(), str.length());
//    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
//    sese::Base64Converter::encode(&input, &output);
//    ASSERT_EQ(std::string_view(buffer), std::string_view("c2VzZS1jb3JlLWxpYnJhcnk="));
//}
//
//TEST(TestBase62, Base62Decode_0) {
//    char buffer[32]{};
//    std::string code = "5L2g5aW9";
//    auto input = sese::InputBufferWrapper(code.c_str(), code.length());
//    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
//    sese::Base64Converter::decode(&input, &output);
//    ASSERT_EQ(std::string_view("你好"), std::string_view(buffer));
//}
//
//TEST(TestBase62, Base62Decode_1) {
//    char buffer[32]{};
//    std::string code = "c2VzZS1jb3JlLWxpYnJhcnk=";
//    auto input = sese::InputBufferWrapper(code.c_str(), code.length());
//    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
//    sese::Base64Converter::decode(&input, &output);
//    ASSERT_EQ(std::string_view("sese-core-library"), std::string_view(buffer));
//}