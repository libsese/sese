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

/// \file HttpClient.h
/// \brief HttpClient
/// \author kaoru
/// \date February 23, 2024

#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

#include <functional>

namespace sese::net::http {
/// HttpClient
class HttpClient {
public:
    using WriteCallback = std::function<int64_t(const void *, size_t)>;
    using ReadCallback = std::function<int64_t(void *, size_t)>;
    using Ptr = std::unique_ptr<HttpClient>;

    /// Create a new HttpClient
    /// \param url URL to request
    /// \param proxy Proxy to use, empty string for no proxy
    /// \return New HttpClient, nullptr if failed
    static Ptr create(const std::string &url, const std::string &proxy = "");

    ~HttpClient();

    /// Execute request
    /// \return Request status
    bool request() const;

    /// Get the last error
    /// \return Last error
    int getLastError() const;

    /// Get the last error string
    /// \return Last error string
    std::string getLastErrorString() const;

    int64_t read(void *buf, size_t len) const;

    int64_t write(const void *buf, size_t len) const;

    /// Get request headers
    /// \return Request headers
    Request::Ptr &getRequest() const;

    /// Get response headers
    /// \return Response headers
    Response::Ptr &getResponse() const;

    /// Set the external source of the request body, this option will reset after the request is completed
    /// @param read_data Source of the body to read
    /// @param expect_total Expected size of the body
    void setReadData(io::PeekableStream *read_data, size_t expect_total) const;

    /// Set the external destination of the response body, this option will reset after the request is completed
    /// @param write_data Destination to receive the body
    void setWriteData(io::OutputStream *write_data) const;

    /// Set the external source of the request body, this option will reset after the request is completed
    /// @param read_callback Callback function to read the body, returns the size read, if incomplete reading, the transfer stops
    /// @param expect_total Expected size of the body
    void setReadCallback(const ReadCallback &read_callback, size_t expect_total) const;

    /// Set the external destination of the response body, this option will reset after the request is completed
    /// @param write_callback Callback function to receive the body, returns the size written, if incomplete writing, the transfer stops
    void setWriteCallback(const WriteCallback &write_callback) const;

private:
    HttpClient() = default;

    class Impl;
    class SSLImpl;
    std::unique_ptr<Impl> impl;
};
} // namespace sese::net::http
