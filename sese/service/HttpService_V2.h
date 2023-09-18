/// \file HttpService_V2.h
/// \brief HTTP 服务，支持 HTTP/1.1 HTTP2
/// \note 本实现暂时缺少流量控制
/// \date 2023年8月24日
/// \author kaoru
/// \version 0.1

#pragma once

#include <sese/service/HttpConnection_V2.h>
#include <sese/net/http/ControllerGroup.h>
#include <sese/net/http/Http2FrameInfo.h>

#include <set>

namespace sese::service::v2 {

    class HttpService;

    /// \brief HTTP 配置
    class API HttpConfig : public TcpTransporterConfig {
        friend class HttpService;
        TcpConnection *createConnection() override;

        std::string workDir = {};
        std::string servProtos = "\x02h2\x08http/1.1";
        std::string servName = HTTPD_NAME;

        /// 用于匹配 Http 键值是否排除索引
        std::set<std::string> excludeIndexedHeader{};
        /// 控制器映射，一般不需要手动更改
        std::map<std::string, net::http::Controller> controllerMap;
        /// 默认控制器，不存在控制器或文件时被调用
        virtual void otherController(net::http::Request &req, net::http::Response &resp);

    public:
        void setKeepAlive(uint32_t keepalive) { TcpTransporterConfig::keepalive = keepalive; }
        void setWorkDir(const std::string &dir) { HttpConfig::workDir = dir; }
        void setServCtx(const security::SSLContext::Ptr &ctx) { TcpTransporterConfig::servCtx = ctx; }
        /// 设置使用 ALPN 协商的协议
        /// \param protos 协议字符串
        void setServProtos(const std::string &protos) { HttpConfig::servProtos = protos; }
        void setServName(const std::string &name) { HttpConfig::servName = name; }
        void setController(const std::string &path, const net::http::Controller &controller);
        void setController(const net::http::ControllerGroup &group);
        /// 添加 Http2 中非索引字段
        /// \param name 排除的字段名称
        void addExcludeIndexedHeader(const std::string &name) { excludeIndexedHeader.emplace(name); }
    };

    /// \brief HTTP 服务
    class API HttpService : public TcpTransporter {
    public:
        explicit HttpService(TcpTransporterConfig *transporterConfig);

    protected:
        void onWrite(event::BaseEvent *event) override;
        void onWriteFile1_1(event::BaseEvent *event) noexcept;
        void onWrite2(event::BaseEvent *event) noexcept;

    protected:
        int onProcAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) override;
        void onProcAlpnGet(TcpConnection *conn, const uint8_t *in, uint32_t inLength) override;
        void onProcHandle(TcpConnection *conn) override;
        void onProcClose(TcpConnection *conn) override;

        /// http/1.1
    protected:
        void onProcHandle1_1(TcpConnection *conn) noexcept;
        void onProcHandleFile1_1(const std::string &path, TcpConnection *conn) noexcept;

        /// http2
    protected:
        bool onProcUpgrade2(TcpConnection *conn) noexcept;
        void onProcHandle2(TcpConnection *conn) noexcept;
        void onProcDispatch2(TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept;
        void onProcHandleFile2(const std::string &path, sese::service::TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept;

        static void requestFromHttp2(net::http::Request &request) noexcept;
        static void response2Http2(net::http::Response &response) noexcept;

        static void writeFrame(uint8_t buffer[9], const net::http::Http2FrameInfo &frame) noexcept;
        void writeHeader(TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept;
        void writeData(TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept;

        void onSettingsFrame(TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept;
        void onWindowUpdate(TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept;
        void onHeaderFrame(TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept;
        void onDataFrame(TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept;
        void onResetFrame(TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept;
    };
}// namespace sese::service::v2