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

#include "sese/io/FakeStream.h"
#include "sese/io/Closeable.h"

#include "gtest/gtest.h"

#include <sstream>

class MyFakeStream {
public:
    int64_t read(void *buffer, size_t len) noexcept {
        return 114;
    }

    int64_t write(const void *buffer, size_t len) noexcept {
        return 514;
    }

    void close() {};
};

TEST(TestFakeStream, FakeStream) {
    MyFakeStream my_fake_stream;
    auto fake_stream = new sese::io::FakeStream<MyFakeStream>(&my_fake_stream);
    sese::io::Stream *stream = fake_stream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);
    delete fake_stream;
}

TEST(TestFakeStream, ClosableFakeStream) {
    MyFakeStream my_fake_stream;
    auto fake_stream = new sese::io::ClosableFakeStream<MyFakeStream>(&my_fake_stream);
    sese::io::Stream *stream = fake_stream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);

    sese::io::Closeable *closeable = fake_stream;
    closeable->close();

    delete fake_stream;
}

TEST(TestFakeStream, StdInputStream) {
    std::string content = "hello";
    std::stringstream stream;
    stream << content;

    std::istream istream(stream.rdbuf());
    auto fake_stream = sese::io::StdInputStreamWrapper(istream);

    char buffer[32]{};
    EXPECT_EQ(content.length(), fake_stream.read(buffer, sizeof(buffer)));
    EXPECT_EQ(content, buffer);
}

TEST(TestFakeStream, StdOutputStream) {
    std::string content = "hello";
    std::stringstream stream;
    std::ostream ostream(stream.rdbuf());
    auto fake_stream = sese::io::StdOutputStreamWrapper(ostream);
    EXPECT_EQ(content.length(), fake_stream.write(content.data(), content.length()));
    EXPECT_EQ(content, stream.str());
}