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

#include "sese/config/ConfigUtil.h"
#include "sese/config/ini/IniUtil.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/io/ConsoleOutputStream.h"

#include "gtest/gtest.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

TEST(TestIni, Read) {
    const char INI[]{
            "server_address=192.168.3.1\n"
            "server_port=8080\n"
            "\n"
            "[video]\n"
            "width = 1920\n"
            "height = 1080"};

    auto input = sese::io::InputBufferWrapper(INI, sizeof(INI) - 1);
    auto config = sese::ConfigUtil::readFrom(&input);
    ASSERT_NE(config, nullptr);

    auto def_config = config->getDefaultSection();
    ASSERT_NE(def_config, nullptr);
    auto address = def_config->getValueByKey("server_address", "undef");
    auto port = def_config->getValueByKey("server_port", "undef");
    EXPECT_EQ(address, "192.168.3.1");
    EXPECT_EQ(port, "8080");

    auto video_config = config->getSectionByName("video");
    ASSERT_NE(video_config, nullptr);
    auto width = video_config->getValueByKey("width", "undef");
    auto height = video_config->getValueByKey("height", "undef");
    EXPECT_EQ(width, "1920");
    EXPECT_EQ(height, "1080");
}

TEST(TestIni, Write) {
    auto config = std::make_shared<sese::ConfigObject>();
    auto def_section = config->getDefaultSection();
    def_section->setKeyValue("name", "test");

    auto section = std::make_shared<sese::ConfigObject::Section>("my_section");
    section->setKeyValue("path", "/usr/bin");
    section->setKeyValue("args", "*");
    config->setSection(section);

    auto output = sese::io::ConsoleOutputStream();
    sese::ConfigUtil::write2(config, &output);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

TEST(TestIniConfig, Parse) {
    auto str = "TestMessage=Hello World\n"
               "# Default Section End\n"
               "\n"
               "[Second Section]\n"
               "Test Message=This is the second section";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));
    auto config = sese::ini::IniUtil::parse(&input);
    // ASSERT_NE(config, nullptr);

    std::string str1 = config->defSection["TestMessage"];
    EXPECT_EQ(str1, "Hello World");

    auto second_section = config->sectionMap["Second Section"];
    EXPECT_EQ(second_section["Test Message"], "This is the second section");
}

TEST(TestIniConfig, Unparse) {
    auto config = std::make_unique<sese::ini::IniConfig>();
    config->defSection["Line1"] = "Content1";
    config->defSection["Line2"] = "Content2";

    auto section2 = sese::ini::IniConfig::Section();
    section2["Line3"] = "Content3";
    section2["Line4"] = "Content4";
    config->sectionMap["Section 2"] = section2;

    auto section3 = sese::ini::IniConfig::Section();
    section3["Line5"] = "Content3";
    section3["Line6"] = "Content4";
    config->sectionMap["Section 3"] = section3;

    sese::io::ConsoleOutputStream output;
    ASSERT_TRUE(sese::ini::IniUtil::unparse(config, &output));
}