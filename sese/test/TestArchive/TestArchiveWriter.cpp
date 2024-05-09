#include <sese/archive/ArchiveWriter.h>
#include <sese/io/File.h>
#include <sese/io/InputBufferWrapper.h>

#include <gtest/gtest.h>

using sese::io::File;
using sese::io::InputBufferWrapper;
using sese::archive::ArchiveWriter;

TEST(TestArchiveWriter, GlobFile) {
    auto file = File::create(PROJECT_BINARY_PATH "/glob.tar.gz", BINARY_WRITE_CREATE_TRUNC);
    ArchiveWriter writer(file.get());
    writer.setFormatTar();
    writer.setFilterGZip();
    EXPECT_TRUE(writer.begin());
    EXPECT_FALSE(writer.addPath(PROJECT_PATH "/null"));
    EXPECT_TRUE(writer.addFile(PROJECT_PATH "/CMakeLists.txt"));
    EXPECT_TRUE(writer.addDirectory(PROJECT_PATH "/sese"));
    EXPECT_TRUE(writer.done());
    file->flush();
    file->close();
}

TEST(TestArchiveWriter, Stream) {
    auto file = File::create(PROJECT_BINARY_PATH "/stream.tar.gz", BINARY_WRITE_CREATE_TRUNC);
    ArchiveWriter writer(file.get());
    writer.setFormatTar();
    writer.setFilterGZip();
    EXPECT_TRUE(writer.begin());
    {
        EXPECT_FALSE(writer.addStream("null.txt", nullptr, 0));
    }
    {
        auto str = "Hello World";
        auto buf = InputBufferWrapper(str, strlen(str));
        EXPECT_TRUE(writer.addStream("Hello.txt", &buf, buf.getCapacity()));
    }
    {
        auto lists = File::create(PROJECT_PATH "/CMakeLists.txt", BINARY_READ_EXISTED);
        EXPECT_TRUE(writer.addStream("CMakeLists.txt", lists.get(),
                                     std::filesystem::file_size(PROJECT_PATH "/CMakeLists.txt")));
        lists->close();
    }
    EXPECT_TRUE(writer.done());
    file->flush();
    file->close();
}

TEST(TestArchiveWriter, Password) {
    auto file = File::create(PROJECT_BINARY_PATH "/password.zip", BINARY_WRITE_CREATE_TRUNC);
    ArchiveWriter writer(file.get());
    writer.setFormatZip();
    // 此选项在 vcpkg 导入的 libarchive 中不受支持，参考链接
    // https://github.com/microsoft/vcpkg/issues/21001
    // https://github.com/libarchive/libarchive/issues/1607
    // https://github.com/libarchive/libarchive/issues/1669
    // 此问题后续将通过远程 ports 将其修复
    // EXPECT_EQ(writer.setOptions("zip:encryption=aes256"), 0) << writer.getErrorString();
    EXPECT_EQ(writer.setOptions("zip:encryption=zipcrypt"), 0) << writer.getErrorString();
    EXPECT_EQ(writer.setPassword("123456"), 0);
    EXPECT_TRUE(writer.begin());
    {
        auto str = "Hello World";
        auto buf = InputBufferWrapper(str, strlen(str));
        EXPECT_TRUE(writer.addStream("Hello.txt", &buf, buf.getCapacity()));
    }
    EXPECT_TRUE(writer.done());
    file->flush();
    file->close();
}