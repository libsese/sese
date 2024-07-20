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

#include <sese/internal/net/http/HttpSSLClientImpl.h>

using sese::internal::net::http::HttpSSLClientImpl;

HttpSSLClientImpl::HttpSSLClientImpl(const sese::net::IPAddress::Ptr &addr, sese::net::http::Request::Ptr req)
    : HttpClientImpl(addr, std::move(req)),
      sslContext(asio::ssl::context::tlsv12),
      sslSocket(socket, sslContext) {
}

bool HttpSSLClientImpl::handshake() {
    code = sslSocket.handshake(asio::ssl::stream_base::client, code);
    return code.value() == 0;
}

bool HttpSSLClientImpl::shutdown() {
    code = sslSocket.shutdown(code);
    return code.value() == 0;
}

int64_t HttpSSLClientImpl::read(void *buffer, size_t length) {
    size_t read = sslSocket.read_some(asio::buffer(buffer, length), code);
    if (code) {
        return -1;
    }
    return static_cast<int64_t>(read);
}

int64_t HttpSSLClientImpl::write(const void *buffer, size_t length) {
    size_t wrote = sslSocket.write_some(asio::buffer(buffer, length), code);
    if (code) {
        return -1;
    }
    return static_cast<int64_t>(wrote);
}
