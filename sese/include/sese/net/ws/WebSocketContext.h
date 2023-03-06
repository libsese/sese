#pragma once

#include "sese/net/ws/FrameHeader.h"
#include "sese/util/Stream.h"

#include <functional>

namespace sese::net::ws {

    class API WebSocketContext {
    public:
        explicit WebSocketContext(Stream *stream);

        bool readFrameInfo(FrameHeaderInfo &info);

        bool writeFrameInfo(const FrameHeaderInfo &info);

        bool ping();

        bool pong();

        int64_t readBinary(void *buf, size_t len);

        int64_t readBinary(void *buf, size_t len, uint32_t maskingKey);

        int64_t writeBinary(const void *buf, size_t len);

        int64_t writeBinary(const void *buf, size_t len, uint32_t maskingKey);

        bool closeNoError();

        bool closeWithError(const void *err, size_t len);

        bool closeWithError(const void *err, size_t len, uint32_t maskingKey);

    protected:
        Stream *stream = nullptr;
    };

}// namespace sese::net::ws