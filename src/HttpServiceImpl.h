#pragma once

#include <optional>
#include <sese/service/http/HttpService_V3.h>

#include "HttpConnection.h"
#include "HttpConnectionEx.h"

/// Http 服务实现
class HttpServiceImpl final : public sese::service::http::v3::HttpService,
                              public std::enable_shared_from_this<HttpServiceImpl> {
public:
    friend struct HttpConnection;

    HttpServiceImpl(
        const sese::net::IPAddress::Ptr &address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterMap &filters
    );

    bool startup() override;

    bool shutdown() override;

    int getLastError() override;

    uint32_t getKeepalive() const { return keepalive; }

    void handleRequest(const HttpConnection::Ptr &conn);

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handleAccept();

    void handleSSLAccept();

    std::set<HttpConnection::Ptr> connections;
};
