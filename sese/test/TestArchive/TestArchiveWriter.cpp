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

#include <sese/archive/ArchiveWriter.h>
#include <sese/io/File.h>
#include <sese/io/InputBufferWrapper.h>

#include <gtest/gtest.h>

using sese::io::File;
using sese::io::InputBufferWrapper;
using sese::archive::ArchiveWriter;

TEST(TestArchiveWriter, GlobFile) {
    auto file = File::create(PROJECT_BINARY_PATH "/glob.tar.gz", File::B_TRUNC);
    ArchiveWriter writer(file.get());
    writer.setFormatTar();
    writer.setFilterGZip();
    EXPECT_TRUE(writer.begin());
    EXPECT_FALSE(writer.addPath(PROJECT_PATH "/null"));
    EXPECT_TRUE(writer.addFile(PROJECT_PATH "/CMakeLists.txt"));
    EXPECT_TRUE(writer.addDirectory(PROJECT_PATH "/sese"));
    EXPECT_TRUE(writer.done());
    EXPECT_EQ(file->flush(), 0);
    file->close();
}

TEST(TestArchiveWriter, Stream) {
    auto file = File::create(PROJECT_BINARY_PATH "/stream.tar.gz", File::B_TRUNC);
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
        auto lists = File::create(PROJECT_PATH "/CMakeLists.txt", sese::io::File::B_READ);
        EXPECT_TRUE(writer.addStream("CMakeLists.txt", lists.get(),
                                     std::filesystem::file_size(PROJECT_PATH "/CMakeLists.txt")));
        lists->close();
    }
    EXPECT_TRUE(writer.done());
    EXPECT_EQ(file->flush(), 0);
    file->close();
}

TEST(TestArchiveWriter, Password) {
    auto file = File::create(PROJECT_BINARY_PATH "/password.zip", File::B_TRUNC);
    ArchiveWriter writer(file.get());
    writer.setFormatZip();
    // This option is not supported in libarchive imported by vcpkg, refer to the link
    // https://github.com/microsoft/vcpkg/issues/21001
    // https://github.com/libarchive/libarchive/issues/1607
    // https://github.com/libarchive/libarchive/issues/1669
    // This issue will be fixed via remote ports in the future
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
    EXPECT_EQ(file->flush(), 0);
    file->close();
}