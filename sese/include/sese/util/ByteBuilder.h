/**
 * @file ByteBuilder.h
 * @brief 线程不安全的字节缓冲类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "AbstractByteBuffer.h"
#include "sese/Config.h"

namespace sese {

    /**
     * @brief 线程不安全的字节缓冲类
     */
    class API ByteBuilder final : public AbstractByteBuffer {
    public:
        using Ptr = std::unique_ptr<ByteBuilder>;

        explicit ByteBuilder(size_t baseSize = STREAM_BYTE_STREAM_SIZE_FACTOR) noexcept : AbstractByteBuffer(baseSize) {
        }
    };

}// namespace sese