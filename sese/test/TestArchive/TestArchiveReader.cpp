#define SESE_C_LIKE_FORMAT

#include <sese/archive/ArchiveReader.h>
#include <sese/record/Marco.h>
#include <sese/io/File.h>

#include <gtest/gtest.h>

using sese::archive::ArchiveReader;
using sese::io::File;

TEST(TestArchiveReader, Extract) {
    EXPECT_TRUE(ArchiveReader::extract(PROJECT_PATH "/sese/test/Data/archive.zip", PROJECT_BINARY_PATH "/archive-p", "password123"));
}

TEST(TestArchiveReader, Extract_wrong_password) {
    EXPECT_FALSE(ArchiveReader::extract(PROJECT_PATH "/sese/test/Data/archive.zip", PROJECT_BINARY_PATH "/archive-p", "wrong password"));
}

TEST(TestArchiveReader, Info) {
    auto file = File::create(PROJECT_PATH "/sese/test/Data/archive.zip", sese::io::FileStream::B_READ);
    ArchiveReader reader(file.get());
    EXPECT_EQ(reader.setPassword("password123"), 0);
    EXPECT_TRUE(reader.extract(
            [](const std::filesystem::path &wpath,
               sese::archive::Config::EntryType type,
               sese::io::InputStream *input,
               size_t size) -> bool {
                auto path = wpath.string();
                SESE_INFO("path: %s size: %zu type: %d", path.c_str(), size, static_cast<int>(type));
                return true;
            }
    ));
    file->close();
}

TEST(TestArchiveReader, Info_wrong_password) {
    auto file = File::create(PROJECT_PATH "/sese/test/Data/archive.zip", sese::io::FileStream::B_READ);
    ArchiveReader reader(file.get());
    EXPECT_EQ(reader.setPassword("wrong password"), 0);
    /// 此处不会出错，能获取到压缩包信息，但理论上无法进行解压
    EXPECT_TRUE(reader.extract(
            [](const std::filesystem::path &wpath,
               sese::archive::Config::EntryType type,
               sese::io::InputStream *input,
               size_t size) -> bool {
                auto path = wpath.string();
                SESE_INFO("path: %s size: %zu type: %d", path.c_str(), size, static_cast<int>(type));
                return true;
            }
    ));
    file->close();
}