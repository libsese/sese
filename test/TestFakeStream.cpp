#include "sese/io/FakeStream.h"
#include "sese/io/Closeable.h"

#include "gtest/gtest.h"

class MyFakeStream {
public:
    int64_t read(void *buffer, size_t len) noexcept {
        return 114;
    }

    int64_t write(const void *buffer, size_t len) noexcept {
        return 514;
    }

    void close(){};
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
