#pragma once

#include "sese/util/ByteBuilder.h"

namespace sese::net::http {

    class Http2Stream : public Stream {
    public:
        using Ptr = std::shared_ptr<Http2Stream>;

        bool willClose = false;
        ByteBuilder buffer;

    public:
        int64_t write(const void *buf, size_t len) override;

        int64_t read(void *buf, size_t len) override;

        void close() override;
    };

}// namespace sese::net::http