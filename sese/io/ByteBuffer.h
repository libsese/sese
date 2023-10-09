/**
 * @file ByteBuffer.h
 * @brief 线程安全的字节缓冲类
 * @author kaoru
 * @date 2022年4月5日
 */
#pragma once

#include "sese/io/AbstractByteBuffer.h"
#include <mutex>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/**
 * @brief 线程安全的字节缓冲类
 */
class API ByteBuffer final : AbstractByteBuffer {
public:
    using Ptr = std::unique_ptr<ByteBuffer>;

    explicit ByteBuffer(size_t baseSize = STREAM_BYTE_STREAM_SIZE_FACTOR, size_t factor = STREAM_BYTE_STREAM_SIZE_FACTOR);
    void resetPos() override;
    [[nodiscard]] size_t getLength();
    [[nodiscard]] size_t getCapacity();
    size_t freeCapacity() override;
    int64_t read(void *buffer, size_t len) override;
    int64_t write(const void *buffer, size_t len) override;
    int64_t peek(void *buffer, size_t len) override;
    int64_t trunc(size_t needRead) override;
    // [[nodiscard]] size_t getCurrentWritePos();
    // [[nodiscard]] size_t getCurrentReadPos();

private:
    using AbstractByteBuffer::getCapacity;
    using AbstractByteBuffer::getLength;

    std::mutex mutex;
};
} // namespace sese::io