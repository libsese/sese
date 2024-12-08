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

#include "sese/io/InputBufferWrapper.h"
#include "sese/io/OutputBufferWrapper.h"
#include "sese/io/FixedBuilder.h"
#include "sese/convert/Base64Converter.h"
#include "gtest/gtest.h"

TEST(TestBase64, Base64Encode_0) {
    char buffer[32]{};
    std::string str = "你好";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("5L2g5aW9"));
}

TEST(TestBase64, Base64Encode_1) {
    char buffer[32]{};
    std::string str = "sese-core-";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("c2VzZS1jb3JlLQ=="));
}

TEST(TestBase64, Base64Encode_2) {
    char buffer[32]{};
    std::string str = "Ciallo Wor";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("Q2lhbGxvIFdvcg=="));
}

TEST(TestBase64, Base64Encode_3) {
    char buffer[32]{};
    std::string str = "Ciallo Worl";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encode(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("Q2lhbGxvIFdvcmw="));
}

TEST(TestBase64, Base64Decode_0) {
    char buffer[32]{};
    std::string code = "5L2g5aW9";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(output.getLength(), 6);
    ASSERT_EQ(std::string_view("你好"), std::string_view(buffer));
}

TEST(TestBase64, Base64Decode_1) {
    char buffer[32]{};
    std::string code = "c2VzZS1jb3JlLQ==";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(output.getLength(), 10);
    ASSERT_EQ(std::string_view("sese-core-"), std::string_view(buffer));
}

TEST(TestBase64, Base64Decode_2) {
    char buffer[32]{};
    std::string code = "Q2lhbGxvIFdvcg==";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(output.getLength(), 10);
    ASSERT_EQ(std::string_view("Ciallo Wor"), std::string_view(buffer));
}

TEST(TestBase64, Base64Decode_3) {
    char buffer[32]{};
    std::string code = "Q2lhbGxvIFdvcmw=";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(output.getLength(), 11);
    ASSERT_EQ(std::string_view("Ciallo Worl"), std::string_view(buffer));
}

TEST(TestBase64, Length) {
    auto input = sese::io::InputBufferWrapper("SGVsbG8=", 8);
    auto output = sese::io::FixedBuilder(32);
    sese::Base64Converter::decode(&input, &output);
    ASSERT_EQ(output.getReadableSize(), 5);
}

TEST(TestBase62, EncodeInteger_0) {
    char buffer[32]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encodeInteger(7562611, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ftXl"));
}

TEST(TestBase62, EncodeInteger_1) {
    char buffer[32]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_TRUE(sese::Base64Converter::encodeInteger(0, &output));
}

TEST(TestBase62, DecodeBuffer_0) {
    const char *code = "ftXl";
    auto num = sese::Base64Converter::decodeBuffer(reinterpret_cast<const unsigned char *>(code), 4);
    ASSERT_EQ(num, 7562611);
}

TEST(TestBase62, DecodeBuffer_1) {
    const char *code = "ftX-";
    auto num = sese::Base64Converter::decodeBuffer(reinterpret_cast<const unsigned char *>(code), 4);
    ASSERT_EQ(num, -1);
}

TEST(TestBase62, Base62Encode_0) {
    char buffer[32]{};
    std::string str = "sese-";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encodeBase62(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ftXlGtv"));
}

TEST(TestBase62, Base62Encode_1) {
    char buffer[32]{};
    std::string str = "sese-core-library";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encodeBase62(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ftXlby7peoC5MeaFbEZbHmp"));
}

TEST(TestBase62, Base62Encode_2) {
    char buffer[32]{};
    std::string str = "core";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::encodeBase62(&input, &output);
    ASSERT_EQ(std::string_view(buffer), std::string_view("bVQWBn"));
}

TEST(TestBase62, Base62Decode_0) {
    char buffer[32]{};
    std::string code = "ftXlGtv";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decodeBase62(&input, &output);
    ASSERT_EQ(std::string_view("sese-"), std::string_view(buffer));
}

TEST(TestBase62, Base62Decode_1) {
    char buffer[32]{};
    std::string code = "ftXlby7peoC5MeaFbEZbHmp";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decodeBase62(&input, &output);
    ASSERT_EQ(std::string_view("sese-core-library"), std::string_view(buffer));
}

TEST(TestBase62, Base62Decode_2) {
    char buffer[32]{};
    std::string code = "bVQWBn";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    sese::Base64Converter::decodeBase62(&input, &output);
    ASSERT_EQ(std::string_view("core"), std::string_view(buffer));
}

/// \brief Encode output buffer insufficient
TEST(TestBase62, Error_0) {
    char buffer[1]{};
    std::string str = "hello";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::Base64Converter::encodeBase62(&input, &output));
}

/// \brief Decode output buffer insufficient
TEST(TestBase62, Error_1) {
    char buffer[1]{};
    std::string str = "bVQWBn";
    auto input = sese::io::InputBufferWrapper(str.c_str(), str.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::Base64Converter::decodeBase62(&input, &output));
}

TEST(TestBase62, Error_2) {
    char buffer[1]{};
    std::string code = "ftXlGtv";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::Base64Converter::decodeBase62(&input, &output));
}

TEST(TestBase62, Error) {
    char buffer[1]{};
    std::string code = "ftXlby7peoC5MeaFbEZbHmp";
    auto input = sese::io::InputBufferWrapper(code.c_str(), code.length());
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::Base64Converter::decodeBase62(&input, &output));
}