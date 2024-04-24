#pragma once

#include <sese/service/http/HttpService_V3.h>

#include <asio.hpp>

namespace sese::internal::service::http::v3 {

struct HttpConnection;
class HttpServiceImpl;

struct HttpConnection {
    using Ptr = std::shared_ptr<HttpConnection>;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &servive, asio::io_context &context);

    net::http::Request request;
    net::http::Response response;

    asio::ip::tcp::socket socket;
    // void *ssl;
    asio::streambuf buffer_reader;
    asio::streambuf buffer_writer;

    size_t expect_length;
    size_t real_length;

    std::shared_ptr<HttpServiceImpl> service;

    static void readNextLine(const HttpConnection::Ptr& conn);
    static void readBody(const HttpConnection::Ptr& conn);
    static void handleRequest(const HttpConnection::Ptr &conn);
    static void writeHeader(const HttpConnection::Ptr &conn);
    static void writeBody(const HttpConnection::Ptr &conn);
};

class HttpServiceImpl final : public sese::service::http::v3::HttpSerivce, public std::enable_shared_from_this<HttpServiceImpl> {
public:
    HttpServiceImpl();

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;

    void handleRequest(const HttpConnection::Ptr &conn);
private:
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handeAccept();
};

} // namespace sese::service::http::v3