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

#include <sese/service/Service.h>
#include <sese/service/HttpSession.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/thread/Thread.h>

#include <map>
#include <functional>
#include <asio.hpp>
#include <asio/ssl.hpp>

namespace sese::internal::service {

class AsioHttpService;
class AsioHttpSession;

class AsioHttpSession : public sese::service::HttpSession {
    friend class AsioHttpService;

public:
    AsioHttpSession(asio::ip::tcp::socket socket);

    ~AsioHttpSession() override;

private:
    bool recv_header = false;
    bool parse_header = false;
    bool recv_body = false;
    bool make_header = false;
    bool send_header = false;
    bool send_body = false;

    sese::iocp::IOBuf recv_buf;
    sese::io::ByteBuilder send_buf;

    size_t recv_body_size{};

    char buffer[MTU_VALUE]{};
    size_t buffer_pos{};
    size_t buffer_length{};

    asio::ip::tcp::socket socket;
    asio::ssl::stream<asio::ip::tcp::socket &> *stream{};
};

class AsioHttpService : public sese::service::Service {
public:
    AsioHttpService(
            net::IPAddress::Ptr addr,
            security::SSLContext::Ptr ssl,
            size_t keep_alive,
            size_t max_buffer_size,
            std::function<void(sese::service::HttpSession *)> callback
    );
    ~AsioHttpService() override;
    bool startup() override;
    bool shutdown() override;
    int getLastError() override;

    void onAsyncAccept(asio::ip::tcp::socket &client);
    void onAsyncRead(const std::shared_ptr<AsioHttpSession> &session, sese::iocp::IOBufNode *node, size_t bytes_transferred);
    void onAsyncWrite(const std::shared_ptr<AsioHttpSession> &session, size_t bytes_transferred);

private:
    net::IPAddress::Ptr addr;
    security::SSLContext::Ptr ssl;
    size_t keep_alive;
    const size_t MAX_BUFFER_SIZE;

    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    asio::ssl::context *ssl_context{};
    asio::error_code code;

    std::map<socket_t, std::shared_ptr<AsioHttpSession>> sessionMap;
    std::function<void(sese::service::HttpSession *)> callback;

    Thread thread;
};
} // namespace sese::internal::service
