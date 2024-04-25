#pragma once

#include <sese/service/http/HttpService_V3.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/io/ByteBuilder.h>

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <optional>

namespace sese::internal::service::http::v3 {

struct HttpConnection;
class HttpServiceImpl;

struct HttpConnection final {
    using Ptr = std::shared_ptr<HttpConnection>;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    net::http::Request request;
    net::http::Response response;

    // 使用 SSL 时所需特定参数
    // is \r ?
    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket &>> stream;
    bool is0x0d = false;
    iocp::IOBuf io_buffer;
    io::ByteBuilder dynamic_buffer;
    char send_buffer[MTU_VALUE]{};

    // 使用非 SSL 时所需特定参数
    asio::ip::tcp::socket socket;
    io::ByteBuilder parse_buffer;
    asio::streambuf asio_dynamic_buffer;

    size_t expect_length;
    size_t real_length;

    std::shared_ptr<HttpServiceImpl> service;

    static void readHeader(const HttpConnection::Ptr& conn);
    static void readBody(const HttpConnection::Ptr& conn);
    static void handleRequest(const HttpConnection::Ptr &conn);
    static void writeHeader(const HttpConnection::Ptr &conn);
    static void writeBody(const HttpConnection::Ptr &conn);
};

class HttpServiceImpl final : public sese::service::http::v3::HttpService, public std::enable_shared_from_this<HttpServiceImpl> {
public:
    HttpServiceImpl();

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;

    void handleRequest(const HttpConnection::Ptr &conn);
private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handeAccept();
};

} // namespace sese::service::http::v3