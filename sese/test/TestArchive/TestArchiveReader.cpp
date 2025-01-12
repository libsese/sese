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

#include <sese/archive/ArchiveReader.h>
#include <sese/log/Marco.h>
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
                SESE_INFO("path: {} size: {} type: {}", path, size, static_cast<int>(type));
                return true;
            }
    ));
    file->close();
}

TEST(TestArchiveReader, Info_wrong_password) {
    auto file = File::create(PROJECT_PATH "/sese/test/Data/archive.zip", sese::io::FileStream::B_READ);
    ArchiveReader reader(file.get());
    EXPECT_EQ(reader.setPassword("wrong password"), 0);
    /// There will be no error here, and the compressed package information can be obtained,
    /// but theoretically it is not possible to decompress it
    EXPECT_TRUE(reader.extract(
            [](const std::filesystem::path &wpath,
               sese::archive::Config::EntryType type,
               sese::io::InputStream *input,
               size_t size) -> bool {
                auto path = wpath.string();
                SESE_INFO("path: {} size: {} type: {}", path, size, static_cast<int>(type));
                return true;
            }
    ));
    file->close();
}