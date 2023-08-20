/// \file HttpConnection_V2.h
/// \brief Http 连接结构
/// \author kaoru
/// \version 0.1
/// \date 日期

#pragma once

#include <sese/service/TcpTransporter.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/net/http/HttpConnection.h>
#include <sese/net/http/DynamicTable.h>
#include <sese/net/http/Http2FrameInfo.h>

namespace sese::service::v2 {

    struct HttpConnection {
        using Ptr = std::shared_ptr<HttpConnection>;

        explicit HttpConnection(net::http::HttpVersion ver) : version(ver) {}
        virtual ~HttpConnection() = default;

        const net::http::HttpVersion version;

        /// 对于 http/1.1 而言，此字段代表着将进行最后一次 http/1.1 的响应
        /// 对于 http2 而言，此字段代表当前连接由 http/1.1 升级而来，需要进行特殊处理
        bool upgrade = false;
    };

    struct Http1_1Connection : public HttpConnection {
        using Ptr = std::shared_ptr<Http1_1Connection>;

        Http1_1Connection() : HttpConnection(net::http::HttpVersion::VERSION_1_1) {}

        uint32_t expect = 0;
        uint32_t real = 0;

        net::http::HttpHandleStatus status = net::http::HttpHandleStatus::HANDING;
        net::http::Request req;
        net::http::Response resp;

        FileStream::Ptr file;
        size_t fileSize = 0;
        std::string contentType = "application/x-";
        std::vector<net::http::Range> ranges;
        std::vector<net::http::Range>::iterator rangeIterator;
    };

    struct Http2Stream {
        using Ptr = std::shared_ptr<Http2Stream>;

        /// 流 ID
        uint32_t id;

        size_t headerSize = 0;
        net::http::HttpHandleStatus status = net::http::HttpHandleStatus::HANDING;
        net::http::Request req;
        net::http::Response resp;

        FileStream::Ptr file;
        size_t fileSize = 0;
        /// 剩余内容长度
        /// 在多分段请求的情况下，contentLength 一定大于 fileSize
        size_t contentLength = 0;
        std::string contentType = "application/x-";
        std::vector<net::http::Range> ranges;
        std::vector<net::http::Range>::iterator rangeIterator;
    };

    struct Http2Connection : public HttpConnection {
        using Ptr = std::shared_ptr<Http2Connection>;

        Http2Connection() : HttpConnection(net::http::HttpVersion::VERSION_2) {}

        int decodeHttp2Settings(const std::string &settings) noexcept;

        std::map<uint32_t, Http2Stream::Ptr> streamMap;
        net::http::DynamicTable dynamicTable1;
        net::http::DynamicTable dynamicTable2;

        uint32_t headerTableSize = 4096;
        uint32_t enablePush = 1;
        uint32_t maxConcurrentStream = 0;
        uint32_t windowSize = 65535;
        uint32_t maxFrameSize = 16384;
        uint32_t maxHeaderListSize = 0;
    };

    struct HttpConnectionWrapper : public TcpConnection {
        std::shared_ptr<HttpConnection> conn;

        bool readFrame(net::http::Http2FrameInfo &frame) noexcept;
        void writeFrame(const net::http::Http2FrameInfo &frame) noexcept;
        void writeAck() noexcept;
    };
}// namespace sese::service::v2