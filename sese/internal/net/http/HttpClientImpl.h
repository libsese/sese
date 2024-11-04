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

/// \file HttpClientImpl.h
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
class HttpClientImpl : public sese::net::http::Requestable {
public:
    HttpClientImpl(const sese::net::IPAddress::Ptr &addr, sese::net::http::Request::Ptr req);

    ~HttpClientImpl() override;

    bool request() override;

    int64_t read(void *buf, size_t len) override;

    int64_t write(const void *buf, size_t len) override;

    int getLastError() override;

    std::string getLastErrorString() override;

protected:
    virtual bool handshake();
    virtual bool shutdown();

    bool writeHeader(io::ByteBuilder &builder);

    bool writeBodyByCallback();
    bool writeBodyByData();
    bool writeBodyByAuto();

    bool readBodyByCallback(size_t expect);
    bool readBodyByData(size_t expect);

    sese::net::IPAddress::Ptr address;
    sese::net::http::CookieMap::Ptr cookies;

    asio::error_code code{};
    asio::io_context ioContext;
    asio::ip::tcp::socket socket;

    bool first = true;
};
} // namespace sese::internal::net::http