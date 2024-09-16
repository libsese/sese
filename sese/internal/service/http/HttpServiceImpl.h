#pragma once

#include <optional>
#include <sese/service/http/HttpService_V3.h>

#include <sese/internal/service/http/HttpConnection.h>
#include <sese/internal/service/http/HttpConnectionEx.h>

namespace sese::internal::service::http {

/// Http 服务实现
class HttpServiceImpl final : public sese::service::http::v3::HttpService,
                              public std::enable_shared_from_this<HttpServiceImpl> {
public:
    friend struct HttpConnection;

    HttpServiceImpl(
        const net::IPAddress::Ptr &address,
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

    void handleRequest(const Handleable::Ptr &conn) const;

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    static constexpr unsigned char alpn_protos[] = "\x2h2\x8http/1.1";

    static int alpnCallback(SSL *ssl, const uint8_t **out, uint8_t *out_length, const uint8_t *in, uint32_t in_length, void *data);

    void handleAccept();

    void handleSSLAccept();

    std::set<HttpConnection::Ptr> connections;
};

}