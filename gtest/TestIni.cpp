#include "sese/config/ConfigUtil.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/ConsoleOutputStream.h"

#include "gtest/gtest.h"

TEST(TestIni, Read) {
    const char ini[] {
        "server_address=192.168.3.1\n"
        "server_port=8080\n"
        "\n"
        "[video]\n"
        "width = 1920\n"
        "height = 1080"
    };

    auto input = sese::InputBufferWrapper(ini, sizeof(ini) - 1);
    auto config = sese::ConfigUtil::readFrom(&input);
    ASSERT_NE(config, nullptr);

    auto defConfig = config->getDefaultSection();
    ASSERT_NE(defConfig, nullptr);
    auto address = defConfig->getValueByKey("server_address", "undef");
    auto port = defConfig->getValueByKey("server_port", "undef");
    EXPECT_EQ(address, "192.168.3.1");
    EXPECT_EQ(port, "8080");

    auto videoConfig = config->getSectionByName("video");
    ASSERT_NE(videoConfig, nullptr);
    auto width = videoConfig->getValueByKey("width", "undef");
    auto height = videoConfig->getValueByKey("height", "undef");
    EXPECT_EQ(width, "1920");
    EXPECT_EQ(height, "1080");
}

TEST(TestIni, Write) {
    auto config = std::make_shared<sese::ConfigObject>();
    auto defSection = config->getDefaultSection();
    defSection->setKeyValue("name", "test");

    auto section = std::make_shared<sese::ConfigObject::Section>("my_section");
    section->setKeyValue("path", "/usr/bin");
    section->setKeyValue("args", "*");
    config->setSection(section);

    auto output = sese::ConsoleOutputStream();
    sese::ConfigUtil::write2(config, &output);
}