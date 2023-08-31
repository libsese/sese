#include "sese/util/BufferedInputStream.h"
#include "sese/util/BufferedOutputStream.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/OutputBufferWrapper.h"
#include "sese/util/ByteBuilder.h"
#include "sese/util/FileStream.h"
#include "sese/util/BaseStreamReader.h"
#include "sese/text/TextReader.h"
#include "gtest/gtest.h"

TEST(TestStream, BufferedInput) {
    auto bytes = std::make_shared<sese::ByteBuilder>(16);
    bytes->write("1234567890", 10);

    char buffer[8]{};
    auto input = sese::BufferedInputStream(bytes);
    input.read(buffer, 4);
    ASSERT_TRUE(0 == std::string("1234").compare(0, 4, buffer, 4));
    input.read(buffer, 2);
    ASSERT_TRUE(0 == std::string("56").compare(0, 2, buffer, 2));
    input.read(buffer, 4);
    ASSERT_TRUE(0 == std::string("7890").compare(0, 4, buffer, 4));
}

TEST(TestStream, BufferedOutput) {
    auto bytes = std::make_shared<sese::ByteBuilder>(16);

    auto output = sese::BufferedOutputStream(bytes);
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
    auto output = sese::OutputBufferWrapper(buffer0, 8);
    auto input = sese::InputBufferWrapper(buffer0, 8);
    output.write("Hello World", 8);
    input.read(buffer1, 16);
    ASSERT_TRUE(0 == std::string("Hello Wo").compare(0, 8, buffer1, 8));
}

TEST(TestStream, StreamReader) {
    auto file = sese::FileStream::create(PROJECT_PATH "/gtest/Data/data-1.txt", TEXT_READ_EXISTED);
    ASSERT_TRUE(file != nullptr);

    auto reader = std::make_shared<sese::StreamReader>(file);
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
    auto reader = sese::text::TextReader::create(PROJECT_PATH "/gtest/Data/data-0.txt");
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

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}