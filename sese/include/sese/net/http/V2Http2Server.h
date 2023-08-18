/// \file V2Http2Server.h
/// \brief 实验性的 Http2 服务（请勿使用）
/// \author kaoru
/// \date 2023年5月5日

#pragma once

#include "sese/net/http/Http2Connection.h"
#include "sese/net/http/Http2FrameInfo.h"
#include "sese/net/http/HttpServer.h"
#include "sese/net/http/HPackUtil.h"

namespace sese::net::v2::http {

    class Http2Server;

    class API [[deprecated]] Http2Context : public InputStream, public OutputStream {
        friend class Http2Server;
    public:
        explicit Http2Context(const net::v2::http::Http2Stream::Ptr &stream, net::http::DynamicTable &table) noexcept;

        int64_t read(void *buffer, size_t length) override;

        int64_t write(const void *buffer, size_t length) override;

        /// 设置响应 HEADER（一次性的）
        /// \param key 键
        /// \param value 值
        void setOnce(const std::string &key, const std::string &value) noexcept;

        /// 设置响应 HEADER（加入索引的）
        /// \param key 键
        /// \param value 值
        void setIndexed(const std::string &key, const std::string &value) noexcept;

        /// 获取请求 HEADER
        /// \param key 键
        /// \param def 默认值
        void get(const std::string &key, const std::string &def) noexcept;

        /// 头部压缩并写入 buffer
        /// \return 头部压缩后大小
        size_t flushHeader() noexcept;

    protected:
        void encodeString(const std::string &str) noexcept;

    protected:
        /// 标识 HEADER 负载大小
        size_t header = 0;
        /// 标识 DATA 负载大小
        size_t data = 0;

        net::http::DynamicTable &table;
        const net::v2::http::Http2Stream::Ptr &stream;
    };

    class API [[deprecated("此实现是实验性且不完整的")]] Http2Server : public Server {
        using HttpContext = sese::net::http::HttpServiceContext<sese::net::v2::IOContext>;
        using Http2Connection = sese::net::v2::http::Http2Connection;
        using Http2Stream = sese::net::v2::http::Http2Stream;
        using Http2FrameInfo = sese::net::http::Http2FrameInfo;
        using HuffmanEncoder = sese::net::http::HuffmanEncoder;
        using HuffmanDecoder = sese::net::http::HuffmanDecoder;
        using Header = sese::net::http::Header;
        using HPackUtil = sese::net::http::HPackUtil;
    public:
        /// 连接可读时对连接进行分发
        /// \param ctx 连接来源
        void onHandle(IOContext &ctx) noexcept override;

        /// 连接关闭时释放相关资源
        /// \param ctx 连接来源
        void onClosing(IOContext &ctx) noexcept override;

        /// Http1 处理
        /// \param ctx 连接来源
        void onHttpHandle(IOContext &ctx) noexcept;

        /// Http2 处理
        /// \param conn 连接
        /// \param first 指示连接是否需要直接处理
        void onHttp2Handle(const net::v2::http::Http2Connection::Ptr &conn, bool first) noexcept;

        virtual void onHttp2Request(Http2Context &ctx) noexcept;

    public:
        /// 尝试读取一帧
        /// \param ctx 帧来源
        /// \param info 存放帧信息
        /// \retval 0 来源无内容
        /// \retval 1 读取成功
        /// \retval -1 读取失败（不完整或者其它原因）
        static int64_t readFrame(IOContext &ctx, Http2FrameInfo &info) noexcept;

        /// 发送一帧
        /// \param conn 目的连接
        /// \param info 帧信息
        /// \return 发送字节数
        static int64_t writeFrame(const Http2Connection::Ptr &conn, Http2FrameInfo &info) noexcept;

        /// 发送 GOAWAY 帧
        /// \param conn 帧目的地
        /// \param sid  Stream ID
        /// \param eid Error ID
        static void sendGoaway(const Http2Connection::Ptr &conn, uint32_t sid, uint32_t eid) noexcept;

        /// 发送 ACK 帧
        /// \param conn 帧目的地
        static void sendACK(const Http2Connection::Ptr &conn) noexcept;

    protected:
        // 将 http 的部分字段转换成 http2 的形式
        static void header2Http2(HttpContext &ctx, Header &header) noexcept;

        // Settings 帧
        static void onSettingsFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Window Update 帧
        static void onWindowUpdateFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Data 帧
        void onDataFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Header 帧
        void onHeadersFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // 用于组装并发送帧
        void onResponseAndSend(const Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept;

        // 发送 Header 帧(s)
        static void sendHeader(Http2Context &ctx, const Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept;

        // 发送 Data 帧(s)
        static void sendData(Http2Context &ctx, const Http2Connection::Ptr &conn, const Http2Stream::Ptr &stream) noexcept;

    protected:
        // 对 connMap 操作加锁
        std::mutex mutex;
        std::map<socket_t, net::v2::http::Http2Connection::Ptr> connMap;
    };

}// namespace sese::net::v2::http