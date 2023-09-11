#include "sese/io/FileStream.h"
#include "sese/io/OutputUtil.h"

#include <gtest/gtest.h>

TEST(TestFileStream, Open) {
    ASSERT_EQ(sese::io::FileStream::create("undef.txt", TEXT_READ_EXISTED), nullptr);
}

TEST(TestFileStream, Seek) {
    auto file = sese::io::FileStream::create("temp1.txt", BINARY_BOTH_CREATE_TRUNC);
    ASSERT_NE(file, nullptr);
    ASSERT_EQ(file << std::string_view("Helle"), 5);
    ASSERT_EQ(file->getSeek(), 5);
    file->setSeek(-1, SEEK_CUR);
    ASSERT_EQ(file->getSeek(), 4);
    ASSERT_EQ(file << std::string_view("o, World"), 8);
    ASSERT_EQ(file->flush(), 0);

    char buffer[16]{};
    file->setSeek(0, SEEK_SET);
    ASSERT_EQ(file->getSeek(), 0);
    ASSERT_EQ(file->read(buffer, 16), 12);
    ASSERT_EQ(std::string_view(buffer), std::string_view("Hello, World"));
    file->close();
}