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

        ~Http1_1Connection() noexcept override;

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

        ~Http2Stream() noexcept;

        /// 流 ID
        uint32_t id;
        /// 如果一个流被 rst_stream，那么将会在最近一次操作该流时移除该流
        bool reset = false;

        size_t headerSize = 0;
        net::http::HttpHandleStatus status = net::http::HttpHandleStatus::HANDING;
        net::http::Request req;
        net::http::Response resp;

        FileStream::Ptr file;
        size_t fileSize = 0;
        /// 剩余内容长度
        /// 在多分段请求的情况下，contentLength 一定大于 fileSize
        size_t contentLength = 0;
        /// 当前帧已处理大小
        size_t currentFramePos = 0;
        /// 当前帧的总大小
        size_t currentFrameSize = 0;
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

        bool first = true;

        uint32_t headerTableSize = 4096;
        uint32_t enablePush = 1;
        uint32_t maxConcurrentStream = 0;
        uint32_t windowSize = 65535;
        uint32_t maxFrameSize = 16384;
        uint32_t maxHeaderListSize = 0;
    };

    struct HttpConnectionWrapper : public TcpConnection {
        std::shared_ptr<HttpConnection> conn;

        /// \brief 从连接中读取一帧的帧头
        /// \note Http2 连接的专有方法
        /// \param frame 帧
        /// \return 是否读取成功
        bool readFrame(net::http::Http2FrameInfo &frame) noexcept;
        /// \brief 向连接中写入一帧的帧头
        /// \note Http2 连接的专有方法
        /// \param frame 帧
        void writeFrame(const net::http::Http2FrameInfo &frame) noexcept;
        /// \brief 向连接中写入 Ack 帧
        /// \note Http2 连接的专有方法
        void writeAck() noexcept;
        /// \brief 向连接中写入 RST 帧
        /// \note Http2 连接的专有方法
        /// \param id 流 ID
        void writeRST(uint32_t id) noexcept;
    };
}// namespace sese::service::v2