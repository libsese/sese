#include <sese/service/iocp/IOBuf.h>

#include <gtest/gtest.h>

TEST(TestIOBuf, Push) {
    sese::iocp::IOBuf buffer;
    EXPECT_EQ(buffer.getReadableSize(), 0);
    EXPECT_EQ(buffer.getTotalSize(), 0);

    auto node = std::make_unique<sese::iocp::IOBufNode>(64);
    node->size = 33;
    buffer.push(std::move(node));
    node = nullptr;
    EXPECT_EQ(buffer.getReadableSize(), 33);
    EXPECT_EQ(buffer.getTotalSize(), 33);

    node = std::make_unique<sese::iocp::IOBufNode>(128);
    node->size = 78;
    buffer.push(std::move(node));
    EXPECT_EQ(buffer.getReadableSize(), 33 + 78);
    EXPECT_EQ(buffer.getTotalSize(), 33 + 78);
}

TEST(TestIOBuf, Clear) {
    sese::iocp::IOBuf buffer;
    auto node = std::make_unique<sese::iocp::IOBufNode>(64);
    node->size = 33;
    buffer.push(std::move(node));
    node = std::make_unique<sese::iocp::IOBufNode>(128);
    node->size = 78;
    buffer.push(std::move(node));
    EXPECT_EQ(buffer.getReadableSize(), 33 + 78);
    EXPECT_EQ(buffer.getTotalSize(), 33 + 78);
    buffer.clear();
    EXPECT_EQ(buffer.getReadableSize(), 0);
    EXPECT_EQ(buffer.getTotalSize(), 0);
}

TEST(TestIOBuf, Input) {
    sese::iocp::IOBuf buffer;
    auto node = std::make_unique<sese::iocp::IOBufNode>(1);
    node->size = 1;
    memcpy(node->buffer, "A", 1);
    buffer.push(std::move(node));
    node = std::make_unique<sese::iocp::IOBufNode>(2);
    node->size = 2;
    memcpy(node->buffer, "BC", 2);
    buffer.push(std::move(node));
    node = std::make_unique<sese::iocp::IOBufNode>(3);
    node->size = 3;
    memcpy(node->buffer, "DEF", 3);
    buffer.push(std::move(node));

    char buf[16]{};
    auto len = buffer.read(buf, 16);
    EXPECT_EQ(std::string(buf), "ABCDEF");
    EXPECT_EQ(len, 6);
    EXPECT_EQ(buffer.getReadableSize(), 0);
    EXPECT_EQ(buffer.getTotalSize(), 6);
}

TEST(TestIOBuf, Peek) {
    sese::iocp::IOBuf buffer;
    auto node = std::make_unique<sese::iocp::IOBufNode>(1);
    node->size = 1;
    memcpy(node->buffer, "A", 1);
    buffer.push(std::move(node));
    node = std::make_unique<sese::iocp::IOBufNode>(2);
    node->size = 2;
    memcpy(node->buffer, "BC", 2);
    buffer.push(std::move(node));
    node = std::make_unique<sese::iocp::IOBufNode>(3);
    node->size = 3;
    memcpy(node->buffer, "DEF", 3);
    buffer.push(std::move(node));

    char buf[16]{};
    auto len = buffer.peek(buf, 16);
    EXPECT_EQ(std::string(buf), "ABCDEF");
    EXPECT_EQ(len, 6);

    EXPECT_EQ(buffer.trunc(2), 2);
    len = buffer.peek(buf, 16);
    EXPECT_EQ(len, 4);
    buf[len] = '\0';
    EXPECT_EQ(std::string(buf), "CDEF");
}
