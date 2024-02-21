#include <sese/convert/EncodingConverter.h>

#include <codecvt>

#include <gtest/gtest.h>

TEST(TestEncoding, String2WString) {
    std::string str = "你好";
    std::wstring wstr = L"你好";

    // ASSERT_EQ(sese::EncodingConverter::toWstring(str), wstr);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    EXPECT_EQ(convert.from_bytes(str), wstr);
}

TEST(TestEncoding, WString2String) {
    std::string str = "你好";
    std::wstring wstr = L"你好";

    // ASSERT_EQ(sese::EncodingConverter::toString(wstr), str);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    EXPECT_EQ(convert.to_bytes(wstr), str);
}