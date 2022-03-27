#pragma once
#include "AbstractByteBuffer.h"
#include "Config.h"

namespace sese {

    class API ByteBuilder : public AbstractByteBuffer {
    public:
        explicit ByteBuilder(size_t baseSize = STREAM_BYTESTREAM_BASE_SIZE) noexcept : AbstractByteBuffer(baseSize) {
        }
    };

}// namespace sese