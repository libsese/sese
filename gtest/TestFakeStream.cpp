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
    MyFakeStream myFakeStream;
    auto fakeStream = new sese::io::FakeStream<MyFakeStream>(&myFakeStream);
    sese::io::Stream *stream = fakeStream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);
    delete fakeStream;
}

TEST(TestFakeStream, ClosableFakeStream) {
    MyFakeStream myFakeStream;
    auto fakeStream = new sese::io::ClosableFakeStream<MyFakeStream>(&myFakeStream);
    sese::io::Stream *stream = fakeStream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);

    sese::io::Closeable *closeable = fakeStream;
    closeable->close();

    delete fakeStream;
}
