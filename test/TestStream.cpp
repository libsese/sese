#include "sese/io/BufferedInputStream.h"
#include "sese/io/BufferedOutputStream.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/io/OutputBufferWrapper.h"
#include "sese/io/ByteBuilder.h"
#include "sese/io/FileStream.h"
#include "sese/io/BaseStreamReader.h"
#include "sese/io/InputStreamReader.h"
#include "sese/text/TextReader.h"
#include "gtest/gtest.h"

TEST(TestStream, BufferedInput) {
    auto bytes = std::make_shared<sese::io::ByteBuilder>(16);
    bytes->write("1234567890", 10);

    char buffer[8]{};
    auto input = sese::io::BufferedInputStream(bytes);
    input.read(buffer, 4);
    ASSERT_TRUE(0 == std::string("1234").compare(0, 4, buffer, 4));
    input.read(buffer, 2);
    ASSERT_TRUE(0 == std::string("56").compare(0, 2, buffer, 2));
    input.read(buffer, 4);
    ASSERT_TRUE(0 == std::string("7890").compare(0, 4, buffer, 4));
}

TEST(TestStream, BufferedOutput) {
    auto bytes = std::make_shared<sese::io::ByteBuilder>(16);

    auto output = sese::io::BufferedOutputStream(bytes);
    output.write("ABC", 3);
    output.write("DEFG", 4);
    output.write("HIJ", 3);
    output.flush();

    char buffer[16]{};
    bytes->read(buffer, 10);
    ASSERT_TRUE(0 == std::string("ABCDEFGHIJ").compare(0, 10, buffer, 10));
}

TEST(TestStream, BufferWrapper) {
    char buffer0[8]{};
    char buffer1[16]{};
    auto output = sese::io::OutputBufferWrapper(buffer0, 8);
    auto input = sese::io::InputBufferWrapper(buffer0, 8);
    output.write("Hello World", 8);
    input.read(buffer1, 16);
    ASSERT_TRUE(0 == std::string("Hello Wo").compare(0, 8, buffer1, 8));
}

TEST(TestStream, StreamReader) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/test/Data/data-1.txt", TEXT_READ_EXISTED);
    ASSERT_TRUE(file != nullptr);

    auto reader = std::make_shared<sese::io::StreamReader>(file);
    std::string line;
    while (true) {
        line = reader->readLine();
        if (line.empty()) {
            break;
        } else {
            puts(line.c_str());
        }
    }
}

TEST(TestStream, TextReader_0) {
    auto reader = sese::text::TextReader::create(PROJECT_PATH "/test/Data/data-0.txt");
    ASSERT_NE(reader, nullptr);

    while (true) {
        auto line = reader->readLine();
        if (line.null()) {
            break;
        } else {
            if (line.empty()) {
                continue;
            } else {
                puts(line.reverse().data());
            }
        }
    }
}

TEST(TestStream, TextReader_1) {
    ASSERT_FALSE(sese::text::TextReader::create("undef.txt"));
}

TEST(TestStream, InputStreamReader) {
    auto buffer = "Line1\r\n"
                  "\n"
                  "Line2\r"
                  "\r"
                  "Line3\n"
                  "";
    auto input = sese::io::InputBufferWrapper(buffer, strlen(buffer));
    auto reader = sese::io::InputStreamReader(&input);

    int count = 0;
    while (true) {
        std::string line = reader.readLine();
        if (line.empty()) {
            break;
        } else {
            count += 1;
            puts(line.c_str());
        }
    }
    EXPECT_EQ(count, 3);
}

#include <sese/io/NullOutputStream.h>

TEST(TestStream, NullOutputStream) {
    auto str = "Null";
    auto null = sese::io::NullOutputStream();
    EXPECT_EQ(null.write(str, strlen(str)), strlen(str));
}

#include <sese/io/RandomInputStream.h>

TEST(TestStream, RandomInputStream) {
    char buffer[32]{};
    auto rand = sese::io::RandomInputStream();
    EXPECT_EQ(rand.read(buffer, sizeof(buffer) - 1), sizeof(buffer) - 1);
}