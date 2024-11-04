// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <optional>
#include <sese/service/http/HttpService.h>

#include <sese/internal/service/http/HttpConnection.h>
#include <sese/internal/service/http/HttpConnectionEx.h>

namespace sese::internal::service::http {

/// Http 服务实现
class HttpServiceImpl final : public sese::service::http::HttpService,
                              public std::enable_shared_from_this<HttpServiceImpl> {
public:
    friend struct HttpConnection;
    friend struct HttpConnectionEx;

    HttpServiceImpl(
        const sese::net::IPAddress::Ptr &address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterCallback &tail_filter,
        FilterMap &filters,
        ConnectionCallback &connection_callback
    );

    bool startup() override;

    bool shutdown() override;

    int getLastError() override;

    std::string getLastErrorMessage() override;

    uint32_t getKeepalive() const { return keepalive; }

    void handleFilter(const Handleable::Ptr &conn) const;

    void handleRequest(const Handleable::Ptr &conn) const;

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    static constexpr unsigned char ALPN_PROTOS[] = "\x2h2\x8http/1.1";

    static int alpnCallback(SSL *ssl, const uint8_t **out, uint8_t *out_length, const uint8_t *in, uint32_t in_length, void *data);

    void handleAccept();

    void handleSSLAccept();

    std::set<HttpConnection::Ptr> connections;
    std::set<HttpConnectionEx::Ptr> connections2;
};

}