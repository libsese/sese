#pragma once

#include <sese/service/Service.h>
#include <sese/net/http/ControllerGroup.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>

#include <asio.hpp>

namespace sese::service::http::v3 {

struct HttpConnection;
class HttpService;

struct HttpConnection {
    using Ptr = std::shared_ptr<HttpConnection>;

    net::http::Request request;
    net::http::Response response;

    asio::ip::tcp::socket socket;
    // void *ssl;
    asio::streambuf buffer_reader;
    asio::streambuf buffer_writer;

    size_t expect_length;
    size_t real_length;

    std::shared_ptr<HttpService> service;

    static void readNextLine(const HttpConnection::Ptr& conn);
    static void readBody(const HttpConnection::Ptr& conn);
    static void handleRequest(const HttpConnection::Ptr &conn);
    static void writeHeader(const HttpConnection::Ptr &conn);
    static void writeBody(const HttpConnection::Ptr &conn);
};

class HttpService : public Service, public std::enable_shared_from_this<HttpService> {
public:
    HttpService();

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;

    HttpService &&setAddress(const net::IPAddress::Ptr &address) && {
        this->address = address;
        return std::move(*this);
    }
    HttpService &&setSSLContext(const security::SSLContext::Ptr &ssl_context) && {
        this->ssl_context = ssl_context;
        return std::move(*this);
    }
    HttpService &&setKeepalive(uint32_t seconds) && {
        this->keepalive = seconds;
        return std::move(*this);
    }
    HttpService &&setName(const std::string &name) && {
        this->serv_name = name;
        return std::move(*this);
    }
    HttpService &&regController(const std::string &http_url, const net::http::Controller &controller) && {
        this->controllers[http_url] = controller;
        return std::move(*this);
    }
    HttpService &&regMount(const std::string &local_src, const std::string &http_url_prefix) && {
        this->mount_points[http_url_prefix] = local_src;
        return std::move(*this);
    }

    void handleRequest(const HttpConnection::Ptr &conn);
private:
    net::IPAddress::Ptr address;
    security::SSLContext::Ptr ssl_context;
    uint32_t keepalive = 30;
    std::string serv_name;
    std::map<std::string, std::string> mount_points;
    std::map<std::string, net::http::Controller> controllers;

    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void startAccept();
};

} // namespace sese::service::http::v3