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