#include <sese/convert/PercentConverter.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

TEST(TestPercentConverter, Normal_0) {
    char buffer[1024]{0};
    const char *string = "Percent Encoding Test | 百分号编码测试";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    sese::PercentConverter::encode(string, dest);
    ASSERT_EQ(std::string_view(buffer),  std::string_view("%50%65%72%63%65%6E%74%20%45%6E%63%6F%64%69%6E%67%20%54%65%73%74%20%7C%20%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81%E6%B5%8B%E8%AF%95"));
}

TEST(TestPercentConverter, Normal_1) {
    char buffer[1024]{0};
    const char *string = "%50%65%72%63%65%6e%74%20%45%6e%63%6f%64%69%6e%67%20%54%65%73%74%20%7c%20%e7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81%E6%B5%8B%E8%AF%95";
    auto dest = std::make_shared<sese::OutputBufferWrapper>(buffer, sizeof(buffer));
    sese::PercentConverter::decode(string, dest);
    ASSERT_EQ(std::string_view(buffer),  std::string_view("Percent Encoding Test | 百分号编码测试"));
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