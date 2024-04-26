#pragma once

#include <sese/service/http/HttpService_V3.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/io/ByteBuilder.h>

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <optional>

namespace sese::internal::service::http::v3 {

struct HttpConnection;
struct HttpConnectionImpl;
struct HttpSSLConnectionImpl;
class HttpServiceImpl;

struct HttpConnection {
    using Ptr = std::shared_ptr<HttpConnection>;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);
    virtual ~HttpConnection() = default;

    net::http::Request request;
    net::http::Response response;
    asio::ip::tcp::socket socket;

    bool keepalive = false;
    asio::system_timer timer;
    void reset();

    size_t expect_length;
    size_t real_length;

    std::shared_ptr<HttpServiceImpl> service;

    virtual void readHeader() = 0;
    virtual void readBody() = 0;
    virtual void handleRequest() = 0;
    virtual void writeHeader() = 0;
    virtual void writeBody() = 0;
    virtual void checkKeepalive() = 0;
};

struct HttpConnectionImpl final : HttpConnection, std::enable_shared_from_this<HttpConnectionImpl> {
    HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    io::ByteBuilder parse_buffer;
    asio::streambuf asio_dynamic_buffer;

    void readHeader() override;
    void readBody() override;
    void handleRequest() override;
    void writeHeader() override;
    void writeBody() override;
    void checkKeepalive() override;
};

struct HttpSSLConnectionImpl final : HttpConnection, std::enable_shared_from_this<HttpSSLConnectionImpl> {
    HttpSSLConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket &>> stream;
    bool is0x0d = false;
    iocp::IOBuf io_buffer;
    io::ByteBuilder dynamic_buffer;
    char send_buffer[MTU_VALUE]{};

    void readHeader() override;
    void readBody() override;
    void handleRequest() override;
    void writeHeader() override;
    void writeBody() override;
    void checkKeepalive() override;
};

class HttpServiceImpl final : public sese::service::http::v3::HttpService, public std::enable_shared_from_this<HttpServiceImpl> {
public:
    HttpServiceImpl();

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;
    uint32_t getKeepalive() const {
        return keepalive;
    }

    void handleRequest(const HttpConnection::Ptr &conn);

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handleAccept();
    void handleSSLAccept();
};

} // namespace sese::internal::service::http::v3