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

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <sese/util/IOBuf.h>
#include <sese/net/http/Range.h>
#include <sese/io/File.h>

#include <sese/internal/service/http/Handleable.h>

namespace sese::internal::service::http {
class HttpServiceImpl;

/// Base implementation of Http connection
struct HttpConnection : Handleable, std::enable_shared_from_this<HttpConnection> {
    using Ptr = std::shared_ptr<HttpConnection>;

    Ptr getPtr() { return shared_from_this(); } // NOLINT

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context, const sese::net::IPAddress::Ptr &addr);

    virtual ~HttpConnection() = default;

    asio::system_timer timer;

    void reset();

    size_t expect_length;
    size_t real_length;
    char send_buffer[MTU_VALUE]{};
    bool is0x0a = false;
    IOBuf io_buffer;
    std::unique_ptr<IOBufNode> node;
    io::ByteBuilder dynamic_buffer;

    std::weak_ptr<HttpServiceImpl> service;

    void readHeader();

    void readBody();

    void handleRequest();

    void writeHeader();

    void writeBody();

    /// Write block function. This function ensures that all buffers are completely written,
    /// and the connection will be disconnected if an unexpected error occurs
    /// @note This function must be implemented
    /// @param buffer Pointer to the buffer
    /// @param length Size of the buffer
    /// @param callback Completion callback function
    virtual void writeBlock(const char *buffer, size_t length,
                            const std::function<void(const asio::error_code &code)> &callback) = 0;

    /// Read function. This function will call the corresponding asio::async_read_some
    /// @param buffer asio::buffer
    /// @param callback Callback function
    virtual void asyncReadSome(const asio::mutable_buffers_1 &buffer,
                               const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &
                               callback) = 0;

    /// Called when a request is completed to determine whether to disconnect the current connection
    /// @note This function must be implemented
    virtual void checkKeepalive() = 0;

    /// Called before the connection is completely released to perform some cleanup of member variables
    /// @note This function is optional to implement
    virtual void disponse();

    void writeSingleRange();

    void writeRanges();
};

/// Http regular connection implementation
struct HttpConnectionImpl final : HttpConnection {
    using Ptr = std::shared_ptr<HttpConnectionImpl>;
    using Socket = asio::ip::tcp::socket;
    using SharedSocket = std::shared_ptr<Socket>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpConnectionImpl>(shared_from_this()); } // NOLINT

    SharedSocket socket;

    HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                       const sese::net::IPAddress::Ptr &addr, SharedSocket socket);

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void asyncReadSome(const asio::mutable_buffers_1 &buffer,
                       const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &
                       callback) override;

    void checkKeepalive() override;
};

/// Http SSL connection implementation
struct HttpsConnectionImpl final : HttpConnection {
    using Ptr = std::shared_ptr<HttpsConnectionImpl>;
    using Stream = asio::ssl::stream<asio::ip::tcp::socket>;
    using SharedStream = std::shared_ptr<Stream>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpsConnectionImpl>(shared_from_this()); } // NOLINT

    SharedStream stream;

    HttpsConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                        const sese::net::IPAddress::Ptr &addr,SharedStream stream);

    ~HttpsConnectionImpl() override;

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void asyncReadSome(const asio::mutable_buffers_1 &buffer,
                       const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &
                       callback) override;

    void checkKeepalive() override;
};

}