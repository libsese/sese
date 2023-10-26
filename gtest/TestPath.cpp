#include <sese/system/Path.h>

#include <gtest/gtest.h>

#ifdef SESE_PLATFORM_WINDOWS

TEST(TestPath, WindowsAbsolutePath) {
    using namespace sese::system;
    auto path = Path("/c/Users/sese/.vcpkg");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getNativePath(), "c:/Users/sese/.vcpkg");
}

TEST(TestPath, WindowsRelativePath) {
    using namespace sese::system;
    auto path = Path(".vcpkg");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getNativePath(), ".vcpkg");
}

TEST(TestPath, WindowsMissingDriver) {
    using namespace sese::system;
    auto path = Path("//vcpkg");
    EXPECT_FALSE(path.isValid());
}

TEST(TestPath, WindowsMissingDriverSplite) {
    using namespace sese::system;
    auto path = Path("/ab/vcpkg");
    EXPECT_FALSE(path.isValid());
}

TEST(TestPath, WindowsParseNativeAbsolutePath){
    using namespace sese::system;
    auto path = Path::fromNativePath("C:/Windows");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getUnixPath(), "/C/Windows");
}

TEST(TestPath, WindowsParseNativeRelativePath){
    using namespace sese::system;
    auto path = Path::fromNativePath("./Windows");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getUnixPath(), "./Windows");
}

TEST(TestPath, WindowsParseUnixPath) {
    using namespace sese::system;
    auto path = Path::fromNativePath("/Windows");
    EXPECT_FALSE(path.isValid());
}

#else


#endif