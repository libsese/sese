// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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