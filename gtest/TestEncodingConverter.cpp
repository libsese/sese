#include <sese/convert/EncodingConverter.h>
#include <gtest/gtest.h>

TEST(TestEncoding, String2WString) {
    std::string str = "你好";
    std::wstring wstr = L"你好";

    ASSERT_EQ(sese::EncodingConverter::toWstring(str), wstr);
}

TEST(TestEncoding, WString2String) {
    std::string str = "你好";
    std::wstring wstr = L"你好";

    ASSERT_EQ(sese::EncodingConverter::toString(wstr), str);
}