#include <sese/system/FileLocker.h>
#include <sese/io/File.h>
#include <sese/thread/Async.h>

#include <gtest/gtest.h>

#include <filesystem>

TEST(TestFileLocker, Lock_1) {
    auto file = sese::io::File::create("temp.txt", sese::io::File::B_TRUNC);
    ASSERT_NE(file, nullptr);

    auto func = [file]() {
        auto fd = file->getFd();
        sese::system::FileLocker locker(fd);
        EXPECT_TRUE(locker.lockWrite(0, 1024));
        file->write("Hello ", 6);
        file->write("World\n", 6);
        EXPECT_EQ(file->flush(), 0);
        EXPECT_TRUE(locker.unlock());
    };

    auto future = sese::async<void>(func);
    future.get();
    func();

    char buffer[1024]{};
    EXPECT_EQ(file->setSeek(0, SEEK_SET), 0);
    file->read(buffer, 1024);
    EXPECT_EQ(std::string("Hello World\nHello World\n"), buffer);
    file->close();

    std::filesystem::remove("temp.txt");
}

TEST(TestFileLocker, Lock_2) {
    auto file = sese::io::File::create("temp.txt", sese::io::File::B_TRUNC);
    ASSERT_NE(file, nullptr);

    auto func = [file]() {
        auto fd = file->getFd();
        sese::system::FileLocker locker(fd);
        EXPECT_TRUE(locker.lock(0, 1024));
        file->write("Hello ", 6);
        file->write("World\n", 6);
        EXPECT_EQ(file->flush(), 0);
        EXPECT_TRUE(locker.unlock());
    };

    auto future = sese::async<void>(func);
    future.get();
    func();

    auto fd = file->getFd();
    sese::system::FileLocker locker(fd);
    EXPECT_TRUE(locker.lockRead(0, 1024));
    char buffer[1024]{};
    EXPECT_EQ(file->setSeek(0, SEEK_SET), 0);
    file->read(buffer, 1024);
    EXPECT_EQ(file->flush(), 0);
    EXPECT_TRUE(locker.unlock());
    EXPECT_EQ(std::string("Hello World\nHello World\n"), buffer);
    file->close();

    std::filesystem::remove("temp.txt");
}