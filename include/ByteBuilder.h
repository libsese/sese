/**
 * @file ByteBuilder.h
 * @brief 线程不安全的字节缓冲类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "AbstractByteBuffer.h"
#include "Config.h"

namespace sese {

    /**
     * @brief 线程不安全的字节缓冲类
     */
    class API ByteBuilder : public AbstractByteBuffer {
    public:
        explicit ByteBuilder(size_t baseSize = STREAM_BYTESTREAM_BASE_SIZE) noexcept : AbstractByteBuffer(baseSize) {
        }
    };

}// namespace sese