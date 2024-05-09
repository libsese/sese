#include <sese/system/Paths.h>
#include <sese/record/Marco.h>

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

TEST(TestPath, UnixConstruct) {
    using namespace sese::system;
    auto path = Path("/usr/bin/ls");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getUnixPath(), "/usr/bin/ls");
    EXPECT_EQ(path.getNativePath(), "/usr/bin/ls");
}

TEST(TestPath, UnixParse) {
    using namespace sese::system;
    auto path = Path::fromNativePath("/usr/bin/ls");
    EXPECT_TRUE(path.isValid());
    EXPECT_EQ(path.getUnixPath(), "/usr/bin/ls");
    EXPECT_EQ(path.getNativePath(), "/usr/bin/ls");
}

#endif

TEST(TestPath, RuntimePaths) {
    auto word_dir = sese::system::Paths::getWorkDir();
    SESE_INFO("work dir: %s", word_dir.getUnixPath().c_str());

    auto exec_dir = sese::system::Paths::getExecutablePath();
    SESE_INFO("exec path: %s", exec_dir.getUnixPath().c_str());

    auto exec_name = sese::system::Paths::getExecutableName();
    SESE_INFO("exec name: %s", exec_name.c_str());
}