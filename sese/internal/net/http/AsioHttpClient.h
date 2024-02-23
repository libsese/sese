#pragma once

#include "sese/net/http/CookieMap.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/Requestable.h"
#include "sese/net/IPv6Address.h"
#include "sese/io/FakeStream.h"
#include "sese/record/Marco.h"

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace sese::net::http {

class AsioHttpClient : public Requestable {
public:
    AsioHttpClient();

    ~AsioHttpClient() override;

    bool init(const std::string &url, const std::string &proxy) override;

    bool request() override;

    int64_t read(void *buf, size_t len) override;

    int64_t write(const void *buf, size_t len) override;

    int getLastError() override;

    std::string getLastErrorString() override;

protected:
    IPAddress::Ptr address = nullptr;
    CookieMap::Ptr cookies = nullptr;

    asio::error_code code{};
    asio::io_context ioContext;
    asio::ip::tcp::socket socket;

    bool ssl = false;
    bool first = true;
    asio::ssl::context sslContext;
    asio::ssl::stream<asio::ip::tcp::socket &> sslSocket;
};
} // namespace sese::net::http