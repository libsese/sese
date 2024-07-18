/// \file AsioHttpClient.h
/// \brief 基于ASIO实现的HTTP/1.1客户端
/// \author kaoru
/// \date 2024年04月10日

#pragma once

#include "sese/net/http/CookieMap.h"
#include "sese/net/http/Requestable.h"
#include "sese/net/IPv6Address.h"
#include "sese/record/Marco.h"

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace sese::internal::net::http {

/// \brief 基于ASIO实现的HTTP/1.1客户端
class AsioHttpClient : public sese::net::http::Requestable {
public:
    AsioHttpClient();

    ~AsioHttpClient() override;

    bool init(const std::string &url, const std::string &proxy) override;

    bool request() override;

    int64_t read(void *buf, size_t len) override;

    int64_t write(const void *buf, size_t len) override;

    int getLastError() override;

    std::string getLastErrorString() override;

private:
    bool writeBodyByCallback();
    bool writeBodyByData();
    bool writeBodyByAuto();

    bool readBodyByCallback(size_t expect);
    bool readBodyByData(size_t expect);

protected:
    sese::net::IPAddress::Ptr address = nullptr;
    sese::net::http::CookieMap::Ptr cookies = nullptr;

    asio::error_code code{};
    asio::io_context ioContext;
    asio::ip::tcp::socket socket;

    bool ssl = false;
    bool first = true;
    asio::ssl::context sslContext;
    asio::ssl::stream<asio::ip::tcp::socket &> sslSocket;
};
} // namespace sese::net::http