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

#include <sese/internal/service/http/HttpServiceImpl.h>
#include <sese/internal/net/AsioIPConvert.h>

sese::internal::service::http::HttpConnectionImpl::HttpConnectionImpl(
        const std::shared_ptr<HttpServiceImpl> &service,
        asio::io_context &context,
        const sese::net::IPAddress::Ptr &addr,
        SharedSocket socket
)
    : HttpConnection(service, context, addr), socket(std::move(socket)) {
}

void sese::internal::service::http::HttpConnectionImpl::writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    async_write(*this->socket, asio::buffer(buffer, length), [conn = shared_from_this(), buffer, length, callback](const asio::error_code &error, std::size_t wrote) {
        if (error || wrote == length) {
            callback(error);
        } else {
            conn->writeBlock(buffer + wrote, length - wrote, callback);
        }
    });
}

void sese::internal::service::http::HttpConnectionImpl::asyncReadSome(const asio::mutable_buffers_1 &buffer, const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &callback) {
    this->socket->async_read_some(buffer, callback);
}

void sese::internal::service::http::HttpConnectionImpl::checkKeepalive() {
    if (this->keepalive) {
        this->reset();
        this->timer.async_wait([conn = getPtr()](const asio::error_code &error) {
            if (error.value() == asio::error::operation_aborted) {
            } else {
                conn->socket->cancel();
                conn->disponse();
            }
        });
        this->readHeader();
    } else {
        this->disponse();
    }
}

sese::internal::service::http::HttpsConnectionImpl::HttpsConnectionImpl(
        const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
        const sese::net::IPAddress::Ptr &addr, SharedStream stream
)
    : HttpConnection(service, context, addr), stream(std::move(stream)) {
    remote_address = net::convert(this->stream->next_layer().remote_endpoint());
}

sese::internal::service::http::HttpsConnectionImpl::~HttpsConnectionImpl() {
    asio::error_code error = this->stream->shutdown(error);
}

void sese::internal::service::http::HttpsConnectionImpl::writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    this->stream->async_write_some(asio::buffer(buffer, length), [conn = getPtr(), buffer, length, callback](const asio::error_code &error, size_t wrote) {
        if (error || wrote == length) {
            callback(error);
        } else {
            conn->writeBlock(buffer + wrote, length - wrote, callback);
        }
    });
}

void sese::internal::service::http::HttpsConnectionImpl::asyncReadSome(const asio::mutable_buffers_1 &buffer, const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &callback) {
    this->stream->async_read_some(buffer, callback);
}

void sese::internal::service::http::HttpsConnectionImpl::checkKeepalive() {
    if (this->keepalive) {
        this->reset();
        this->timer.async_wait([conn = getPtr()](const asio::error_code &error) {
            if (error == asio::error::operation_aborted) {
            } else {
                conn->stream->lowest_layer().cancel();
                conn->disponse();
            }
        });
        this->readHeader();
    } else {
        this->disponse();
    }
}
