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

/// \file HttpSSLClientImpl.h
/// \brief 基于ASIO实现的HTTP/1.1客户端
/// \author kaoru
/// \date 2024年04月10日

#pragma once

#include <sese/internal/net/http/HttpClientImpl.h>

namespace sese::internal::net::http {

/// \brief 基于ASIO实现的HTTP/1.1客户端
class HttpSSLClientImpl final : public HttpClientImpl {
public:
    HttpSSLClientImpl(const sese::net::IPAddress::Ptr &addr, sese::net::http::Request::Ptr req);

protected:
    bool handshake() override;

    bool shutdown() override;

    int64_t read(void *buffer, size_t length) override;

    int64_t write(const void *buffer, size_t length) override;

private:
    asio::ssl::context sslContext;
    asio::ssl::stream<asio::ip::tcp::socket &> sslSocket;
};

} // namespace sese::internal::net::http
