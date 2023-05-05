/// \file V2Http2Server.h
/// \brief 实验性的 Http2 服务（请勿使用）
/// \author kaoru
/// \date 2023年5月5日

#pragma once

#include "sese/net/http/Http2Connection.h"
#include "sese/net/http/Http2FrameInfo.h"
#include "sese/net/http/HttpServer.h"
#include "sese/net/http/Huffman.h"

namespace sese::net::v2::http {

    class API [[deprecated("此实现是实验性且不完整的")]] Http2Server : public Server {
        using HttpContext = sese::net::http::HttpServiceContext<sese::net::v2::IOContext>;
        using Http2Connection = sese::net::http::Http2Connection;
        using Http2Stream = sese::net::http::Http2Stream;
        using Http2FrameInfo = sese::net::http::Http2FrameInfo;
        using DynamicTable = sese::net::http::DynamicTable;
        using HuffmanEncoder = sese::net::http::HuffmanEncoder;
        using HuffmanDecoder = sese::net::http::HuffmanDecoder;
        using Header = sese::net::http::Header;
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
        void onHttp2Handle(const Http2Connection::Ptr &conn, bool first) noexcept;

        virtual void onHttp2Request(const Http2Stream::Ptr &stream) noexcept;

    public:
        /// 尝试读取一帧
        /// \param ctx 帧来源
        /// \param info 存放帧信息
        /// \retval 0 来源无内容
        /// \retval 1 读取成功
        /// \retval -1 读取失败（不完整或者其它原因）
        static int64_t readFrame(IOContext &ctx, Http2FrameInfo &info) noexcept;

        /// 发送 GOAWAY 帧
        /// \param conn 帧目的地
        /// \param sid  Stream ID
        /// \param eid Error ID
        static void sendGoaway(const Http2Connection::Ptr &conn, uint32_t sid, uint32_t eid) noexcept;

        /// 发送 ACK 帧
        /// \param conn 帧目的地
        static void sendACK(const Http2Connection::Ptr &conn) noexcept;

        /// 解码 Header 帧负载
        /// \param input 来源
        /// \param dynamicTable 动态表
        /// \param header 输出头部
        /// \return 解码是否成功
        bool decode(InputStream *input, DynamicTable &dynamicTable, Header &header) noexcept;

    protected:
        // 解码整型数字
        static void decodeInteger(uint8_t &buf, InputStream *input, uint32_t &dest, uint8_t n) noexcept;

        // 解码字符串
        std::optional<std::string> decodeString(InputStream *input) noexcept;

        // 将 http 的部分字段转换成 http2 的形式
        static void header2Http2(HttpContext &ctx, Header &header) noexcept;

        // Settings 帧
        static void onSettingsFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Window Update 帧
        static void onWindowUpdateFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Data 帧
        static void onDataFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

        // Header 帧
        void onHeadersFrame(Http2FrameInfo &info, const Http2Connection::Ptr &conn) noexcept;

    protected:
        // 对 connMap 操作加锁
        std::mutex mutex;
        std::map<socket_t, net::http::Http2Connection::Ptr> connMap;

        HuffmanDecoder decoder;
        HuffmanEncoder encoder;
    };

}// namespace sese::net::v2::http