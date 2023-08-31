#include "sese/util/FakeStream.h"
#include "sese/util/Closeable.h"

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
    auto fakeStream = new sese::FakeStream<MyFakeStream>(&myFakeStream);
    sese::Stream *stream = fakeStream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);
    delete fakeStream;
}

TEST(TestFakeStream, ClosableFakeStream) {
    MyFakeStream myFakeStream;
    auto fakeStream = new sese::ClosableFakeStream<MyFakeStream>(&myFakeStream);
    sese::Stream *stream = fakeStream;
    ASSERT_EQ(stream->read(nullptr, 0), 114);
    ASSERT_EQ(stream->write(nullptr, 0), 514);

    sese::Closeable *closeable = fakeStream;
    closeable->close();

    delete fakeStream;
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}
