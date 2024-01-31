#include <sese/res/Marco.h>
#include <gtest/gtest.h>

SESE_DEF_RES_MANAGER(TestResource)
    SESE_ADD_RES("hello.txt", sizeof("Hello"), "Hello")
SESE_DEF_RES_MANAGER_END(TestResource)

TEST(TestResource, Manager) {
    // 获取资源管理器实例
    auto instance = TestResourceInstance::getInstance();
    EXPECT_NE(instance ,nullptr);

    // 获取资源实例
    auto res = instance->getResouce("hello.txt");
    EXPECT_NE(res, nullptr);

    // 获取资源流
    auto stream = res->getStream();
    EXPECT_NE(stream, nullptr);

    char buffer[16]{};
    EXPECT_EQ(stream->read(buffer, stream->getSize()), 5);
    EXPECT_EQ(std::string_view(buffer), "Hello");
}
