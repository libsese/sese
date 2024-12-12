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

/// \file Requestable.h
/// \brief Requestable Interface
/// \author kaoru
/// \date February 23, 2024

#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/io/InputStream.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

#include <functional>

namespace sese::net::http {
/// Requestable Interface
class Requestable : public io::InputStream, public io::OutputStream {
public:
    using WriteCallback = std::function<int64_t(const void *, size_t)>;
    using ReadCallback = std::function<int64_t(void *, size_t)>;

    ~Requestable() override = default;

    /// Execute request
    /// \return Request status
    virtual bool request() = 0;

    /// Get the last error
    /// \return Last error
    virtual int getLastError() = 0;

    /// Get the last error string
    /// \return Last error string
    virtual std::string getLastErrorString() = 0;

    int64_t read(void *buf, size_t len) override = 0;

    int64_t write(const void *buf, size_t len) override = 0;

    /// Get request headers
    /// \return Request headers
    Request::Ptr &getRequest() { return req; }

    /// Get response headers
    /// \return Response headers
    Response::Ptr &getResponse() { return resp; }

    /// Set the external source of the request body, this option will reset after the request is completed
    /// @param read_data Source of the body to read
    /// @param expect_total Expected size of the body
    void setReadData(io::PeekableStream *read_data, size_t expect_total) {
        this->read_data = read_data;
        this->expect_total = expect_total;
    }

    /// Set the external destination of the response body, this option will reset after the request is completed
    /// @param write_data Destination to receive the body
    void setWriteData(io::OutputStream *write_data) {
        this->write_data = write_data;
    }

    /// Set the external source of the request body, this option will reset after the request is completed
    /// @param read_callback Callback function to read the body, returns the size read, if incomplete reading, the transfer stops
    /// @param expect_total Expected size of the body
    void setReadCallback(const ReadCallback &read_callback, size_t expect_total) {
        this->read_callback = read_callback;
        this->expect_total = expect_total;
    }

    /// Set the external destination of the response body, this option will reset after the request is completed
    /// @param write_callback Callback function to receive the body, returns the size written, if incomplete writing, the transfer stops
    void setWriteCallback(const WriteCallback &write_callback) {
        this->write_callback = write_callback;
    }

    /// Reset body-related settings
    void reset() {
        expect_total = 0;
        read_data = &req->getBody();
        write_data = &resp->getBody();
        read_callback = nullptr;
        write_callback = nullptr;
    }

protected:
    Request::Ptr req = nullptr;
    Response::Ptr resp = nullptr;

    size_t expect_total = 0;

    io::PeekableStream *read_data = nullptr;
    io::OutputStream *write_data = nullptr;

    WriteCallback write_callback;
    ReadCallback read_callback;
};
} // namespace sese::net::http
