#pragma once

#include "sese/net/http/Header.h"
#include "sese/util/ByteBuilder.h"

namespace sese::net::http {

    class Http2Stream : public Stream {
    public:
        using Ptr = std::shared_ptr<Http2Stream>;

        bool willClose = false;
        ByteBuilder buffer;

        /// 流请求头部
        Header requestHeader;
        /// 流响应头部 - 非索引的
        Header responseOnceHeader;
        /// 流响应头部 - 索引的
        Header responseIndexedHeader;

    public:
        uint32_t sid = 0;

        int64_t write(const void *buf, size_t len) override;

        int64_t read(void *buf, size_t len) override;

        void close() override;
    };

}// namespace sese::net::http