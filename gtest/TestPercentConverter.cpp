#include <sese/convert/PercentConverter.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

TEST(TestPercentConverter, Normal_0) {
    char buffer[1024]{0};
    const char *string = "Percent百分号Encoding编码Test测试";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    sese::PercentConverter::encode(string, dest);
    ASSERT_EQ(std::string_view(buffer),  std::string_view("Percent%E7%99%BE%E5%88%86%E5%8F%B7Encoding%E7%BC%96%E7%A0%81Test%E6%B5%8B%E8%AF%95"));
}

TEST(TestPercentConverter, Normal_1) {
    char buffer[1024]{0};
    const char *string = "Percent%E7%99%BE%E5%88%86%E5%8F%B7Encoding%E7%BC%96%E7%A0%81Test%E6%B5%8B%E8%AF%95";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    sese::PercentConverter::decode(string, dest);
    ASSERT_EQ(std::string_view(buffer),  std::string_view("Percent百分号Encoding编码Test测试"));
}

TEST(TestPercentConverter, Normal_2) {
    char buffer[1024]{0};
    const char *string = "where=anywhere&这是=测试";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    sese::PercentConverter::encode(string, dest, sese::PercentConverter::urlExcludeChars);
    ASSERT_EQ(std::string_view(buffer),  std::string_view("where%3Danywhere%26%E8%BF%99%E6%98%AF%3D%E6%B5%8B%E8%AF%95"));
}

TEST(TestPercentConverter, Error_0) {
    char buffer[1024]{0};
    const char *string = "|50%RA";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::PercentConverter::decode(string, dest));
}

TEST(TestPercentConverter, Error_1) {
    char buffer[1024]{0};
    const char *string = "%50%RA";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::PercentConverter::decode(string, dest));
}

TEST(TestPercentConverter, Error_2) {
    char buffer[1024]{0};
    const char *string = "%50%AG";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    ASSERT_FALSE(sese::PercentConverter::decode(string, dest));
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}