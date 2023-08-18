#pragma once

#include "sese/net/http/Request.h"
#include "sese/net/http/Response.h"
#include "sese/util/Closeable.h"
#include "sese/util/ByteBuilder.h"

namespace sese::net::v2::http {

    class [[deprecated]] Http2Stream : public Stream, public Closeable {
    public:
        using Ptr = std::shared_ptr<Http2Stream>;

        bool willClose = false;
        ByteBuilder buffer;

        /// 流请求头部
        net::http::Header requestHeader;
        /// 流响应头部 - 非索引的
        net::http::Header responseOnceHeader;
        /// 流响应头部 - 索引的
        net::http::Header responseIndexedHeader;

    public:
        uint32_t sid = 0;

        int64_t write(const void *buf, size_t len) override;

        int64_t read(void *buf, size_t len) override;

        void close() override;
    };

}// namespace sese::net::v2::http

namespace sese::net::http {
    struct API Http2Stream {
        using Ptr = std::shared_ptr<Http2Stream>;

        uint32_t id = 0;
        size_t headerSize = 0;

        Request req;
        Response resp;
    };
}// namespace sese::net::http