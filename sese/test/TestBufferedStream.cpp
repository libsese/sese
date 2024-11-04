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

#include "sese/io/BufferedStream.h"
#include "sese/io/BufferedInputStream.h"
#include "sese/io/BufferedOutputStream.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/io/OutputBufferWrapper.h"
#include "sese/io/ByteBuilder.h"
#include "sese/io/FixedBuilder.h"

#include "gtest/gtest.h"

TEST(TestBufferedStream, Output_0) {
    using sese::io::BufferedOutputStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    auto buffered = BufferedOutputStream(bytes, 16);
    ASSERT_EQ(buffered.getCapacity(), 16);
    buffered.write("Hello, World", 12);
    ASSERT_EQ(buffered.getLength(), 12);
    ASSERT_EQ(bytes->getLength(), 0);
    buffered.write("World", 5);
    buffered.write("HelloHelloHelloHello", 20);
    ASSERT_EQ(buffered.getLength(), 0);
    ASSERT_EQ(bytes->getLength(), 37);
}

TEST(TestBufferedStream, Output_1) {
    using sese::io::BufferedOutputStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedOutputStream(bytes, 6);

    ASSERT_EQ(buffered.write("ABCDE", 5), 5);

    ASSERT_EQ(buffered.write("ABCDE", 5), -1);
}

TEST(TestBufferedStream, Output_2) {
    using sese::io::BufferedOutputStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedOutputStream(bytes, 6);

    ASSERT_EQ(buffered.write("ABCDE", 5), 5);
    ASSERT_EQ(buffered.write("ABCDEFG", 7), -1);
}

TEST(TestBufferedStream, Output_3) {
    using sese::io::BufferedOutputStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedOutputStream(bytes, 6);

    ASSERT_EQ(buffered.write("ABC", 3), 3);
    ASSERT_EQ(buffered.write("ABCDEFG", 7), -1);
}

TEST(TestBufferedStream, Output_4) {
    using sese::io::BufferedOutputStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    auto buffered = BufferedOutputStream(bytes, 4);
    ASSERT_EQ(buffered.write("ABCDE", 5), 5);
}

TEST(TestBufferedStream, Input_0) {
    using sese::io::BufferedInputStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    bytes->write("Hello, World, Hello, World, Hello, World", 40);
    auto buffered = BufferedInputStream(bytes, 8);

    char buffer[1024]{};

    buffered.read(buffer, 1);
    ASSERT_EQ(buffered.getLength(), 8);
    ASSERT_EQ(buffered.getPosition(), 1);
    ASSERT_EQ(buffer[0], 'H');

    buffered.read(buffer, 7);
    ASSERT_EQ(buffered.getLength(), 8);
    ASSERT_EQ(buffered.getPosition(), 8);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ello, W"));

    buffered.read(buffer, 30);
    ASSERT_EQ(std::string_view(buffer), std::string_view("orld, Hello, World, Hello, Wor"));

    buffered.read(buffer, 3);
    buffer[2] = 0;
    ASSERT_EQ(std::string_view(buffer), std::string_view("ld"));

    auto len = buffered.read(buffer, 6);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferedStream, Input_1) {
    using sese::io::BufferedInputStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    bytes->write("Hello, World", 12);
    auto buffered = BufferedInputStream(bytes, 8);

    char buffer[1024]{};
    ASSERT_EQ(buffered.read(buffer, 4), 4);
    ASSERT_EQ(buffered.read(buffer, 12), 8);
}

TEST(TestBufferedStream, BufferedStream_0) {
    using sese::io::BufferedStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    auto buffered = BufferedStream(bytes, 16);
    ASSERT_EQ(buffered.getCapacity(), 16);
    buffered.write("Hello, World", 12);
    ASSERT_EQ(buffered.getLength(), 12);
    ASSERT_EQ(bytes->getLength(), 0);
    buffered.write("World", 5);
    buffered.write("HelloHelloHelloHello", 20);
    ASSERT_EQ(buffered.getLength(), 0);
    ASSERT_EQ(bytes->getLength(), 37);
    buffered.clear();
}

TEST(TestBufferedStream, BufferedStream_1) {
    using sese::io::BufferedStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedStream(bytes, 6);

    ASSERT_EQ(buffered.write("ABCDE", 5), 5);

    ASSERT_EQ(buffered.write("ABCDE", 5), -1);
}

TEST(TestBufferedStream, BufferedStream_2) {
    using sese::io::BufferedStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedStream(bytes, 6);

    ASSERT_EQ(buffered.write("ABCDE", 5), 5);
    ASSERT_EQ(buffered.write("ABCDEFG", 7), -1);
}

TEST(TestBufferedStream, BufferedStream_3) {
    using sese::io::BufferedStream;
    using sese::io::FixedBuilder;

    auto bytes = std::make_shared<FixedBuilder>(4);
    auto buffered = BufferedStream(bytes, 4);

    ASSERT_EQ(buffered.write("ABC", 3), 3);
    ASSERT_EQ(buffered.write("ABCDEFG", 7), -1);
}

TEST(TestBufferedStream, BufferedStream_4) {
    using sese::io::BufferedStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    auto buffered = BufferedStream(bytes, 4);
    ASSERT_EQ(buffered.write("ABCDE", 5), 5);
}

TEST(TestBufferedStream, BufferedStream_5) {
    using sese::io::BufferedStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    bytes->write("Hello, World, Hello, World, Hello, World", 40);
    auto buffered = BufferedStream(bytes, 8);

    char buffer[1024]{};

    buffered.read(buffer, 1);
    ASSERT_EQ(buffered.getLength(), 8);
    ASSERT_EQ(buffered.getPosition(), 1);
    ASSERT_EQ(buffer[0], 'H');

    buffered.read(buffer, 7);
    ASSERT_EQ(buffered.getLength(), 8);
    ASSERT_EQ(buffered.getPosition(), 8);
    ASSERT_EQ(std::string_view(buffer), std::string_view("ello, W"));

    buffered.read(buffer, 30);
    ASSERT_EQ(std::string_view(buffer), std::string_view("orld, Hello, World, Hello, Wor"));

    buffered.read(buffer, 3);
    buffer[2] = 0;
    ASSERT_EQ(std::string_view(buffer), std::string_view("ld"));

    auto len = buffered.read(buffer, 6);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferedStream, BufferedStream_6) {
    using sese::io::BufferedStream;
    using sese::io::ByteBuilder;

    auto bytes = std::make_shared<ByteBuilder>(1024);
    bytes->write("Hello, World", 12);
    auto buffered = BufferedStream(bytes, 8);

    char buffer[1024]{};
    ASSERT_EQ(buffered.read(buffer, 4), 4);
    ASSERT_EQ(buffered.read(buffer, 12), 8);
}