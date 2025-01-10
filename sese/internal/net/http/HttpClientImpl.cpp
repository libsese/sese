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

/// \file HttpClientImpl.cpp
/// \brief HTTP/1.1 client based on ASIO
/// \author kaoru
/// \date April 10, 2024

#include "sese/net/http/CookieMap.h"
#include "sese/net/http/HttpClient.h"
#include "sese/net/IPv6Address.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/record/Marco.h"
#include "sese/io/ByteBuilder.h"
#include "sese/util/Util.h"
#include "sese/internal/net/AsioIPConvert.h"
#include "sese/net/http/RequestParser.h"

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace sese::net::http {

/// \brief HTTP/1.1 client based on ASIO
class HttpClient::Impl : public io::InputStream, public io::OutputStream {
public:
    Impl(const IPAddress::Ptr &addr, Request::Ptr req)
        : socket(ioContext) {
        address = addr;
        this->req = std::move(req);
        cookies = std::make_shared<CookieMap>();
        this->req->setCookies(cookies);
        resp = std::make_unique<Response>();
        reset();
    }

    ~Impl() override {
        socket.close();
    }

    bool request() {
        // Handle the length of the content
        char *end;
        if (expect_total) {
            req->set("content-length", std::to_string(expect_total));
        } else {
            req->set("content-length", std::to_string(req->getBody().getLength()));
        }

        // Allow one reconnection attempt
        int times = 0;
        if (first) {
            times += 1;
        } else {
            first = false;
        }
        io::ByteBuilder bytes;
        HttpUtil::sendRequest(&bytes, req.get());
        while (!writeHeader(bytes)) {
            if (times > 1) {
                reset();
                return false;
            }

            times += 1;
            asio::ip::tcp::endpoint endpoint(internal::net::convert(address), address->getPort());
            code = socket.connect(endpoint, code);
            if (code) {
                reset();
                return false;
            }

            if (!handshake()) {
                reset();
                return false;
            }
        }

        // Determine how to read the body
        if (expect_total && read_callback) {
            if (!writeBodyByCallback()) {
                reset();
                return false;
            }
        } else if (expect_total) {
            if (!writeBodyByData()) {
                reset();
                return false;
            }
        } else if (req->getBody().getLength()) {
            if (!writeBodyByAuto()) {
                reset();
                return false;
            }
        }

        auto response_status = HttpUtil::recvResponse(this, resp.get());
        if (!response_status) {
            reset();
            return false;
        }

        // Determine how to write to the body
        auto expect = std::strtol(resp->get("content-length", "0").c_str(), &end, 10);
        if (req->getType() != RequestType::HEAD && expect > 0) {
            if (write_callback && !readBodyByCallback(expect)) {
                reset();
                return false;
            }
            if (!readBodyByData(expect)) {
                reset();
                return false;
            }
        }

        const auto CONNECTION_VALUE = resp->get("connection", "close");
        if (strcmpDoNotCase("close", CONNECTION_VALUE.c_str())) {
            first = true;
            shutdown();
            socket.close();
        }

        // Automatic application of cookies
        const auto DEST = req->getCookies();
        for (auto &[name, value]: *resp->getCookies()) { // NOLINT
            DEST->add(value);
        }

        reset();
        return true;
    }

    /// Reset body-related settings
    void reset() {
        expect_total = 0;
        read_data = &req->getBody();
        write_data = &resp->getBody();
        read_callback = nullptr;
        write_callback = nullptr;
    }

    virtual bool handshake() {
        return true;
    }

    virtual bool shutdown() {
        return true;
    }

    bool writeHeader(io::ByteBuilder &builder) {
        builder.resetPos();
        while (true) {
            char buffer[MTU_VALUE];
            auto len = builder.peek(buffer, MTU_VALUE);
            if (len == 0) {
                return true;
            }
            auto wrote = this->write(buffer, len);
            if (wrote <= 0) {
                return false;
            }
            builder.trunc(wrote);
        }
    }

    bool writeBodyByCallback() {
        size_t real = 0;
        while (true) {
            if (real >= expect_total) {
                return true;
            }
            auto need = std::min<size_t>(expect_total - real, MTU_VALUE);
            char buffer[MTU_VALUE];
            auto len = read_callback(buffer, need);
            if (len <= 0) {
                return false;
            }
            auto wrote = this->write(buffer, len);
            if (len != wrote) {
                return false;
            }
            real += wrote;
        }
    }

    bool writeBodyByData() {
        size_t real = 0;
        while (true) {
            if (real >= expect_total) {
                return true;
            }
            auto need = std::min<size_t>(expect_total - real, MTU_VALUE);
            char buffer[MTU_VALUE];
            auto len = read_data->peek(buffer, need);
            auto wrote = this->write(buffer, len);
            if (wrote <= 0) {
                return false;
            }
            read_data->trunc(wrote);
            real += wrote;
        }
    }

    bool writeBodyByAuto() {
        while (true) {
            char buffer[MTU_VALUE];
            auto len = req->getBody().peek(buffer, MTU_VALUE);
            if (len == 0) {
                return true;
            }
            auto wrote = this->write(buffer, len);
            if (wrote <= 0) {
                return false;
            }
            req->getBody().trunc(wrote);
        }
    }

    bool readBodyByCallback(size_t expect) {
        size_t real = 0;
        while (true) {
            if (real >= expect) {
                return true;
            }
            char buffer[MTU_VALUE];
            auto need = std::min<size_t>(expect - real, MTU_VALUE);
            auto read = this->read(buffer, need);
            if (read <= 0) {
                return false;
            }
            if (read != write_callback(buffer, read)) {
                return false;
            }
            real += read;
        }
    }

    bool readBodyByData(size_t expect) {
        size_t real = 0;
        while (true) {
            if (real >= expect) {
                return true;
            }
            char buffer[MTU_VALUE];
            auto need = std::min<size_t>(expect - real, MTU_VALUE);
            auto read = this->read(buffer, need);
            if (read <= 0) {
                return false;
            }
            if (read != write_data->write(buffer, read)) {
                return false;
            }
            real += read;
        }
    }

    int64_t read(void *buf, size_t len) override {
        size_t read = socket.read_some(asio::buffer(buf, len), code);
        if (code) {
            return -1;
        }
        return static_cast<int64_t>(read);
    }

    int64_t write(const void *buf, size_t len) override {
        size_t wrote = socket.write_some(asio::buffer(buf, len), code);
        if (code) {
            return -1;
        }
        return static_cast<int64_t>(wrote);
    }

    int getLastError() const {
        return code.value();
    }

    std::string getLastErrorString() const {
        return code.message();
    }

    IPAddress::Ptr address;
    CookieMap::Ptr cookies;

    asio::error_code code{};
    asio::io_context ioContext;
    asio::ip::tcp::socket socket;

    bool first = true;

    Request::Ptr req = nullptr;
    Response::Ptr resp = nullptr;

    size_t expect_total = 0;

    io::PeekableStream *read_data = nullptr;
    OutputStream *write_data = nullptr;

    WriteCallback write_callback;
    ReadCallback read_callback;
};

/// \brief HTTP/1.1 client based on ASIO
class HttpClient::SSLImpl final : public Impl {
public:
    SSLImpl(const IPAddress::Ptr &addr, Request::Ptr req)
        : Impl(addr, std::move(req)),
          sslContext(asio::ssl::context::tlsv12),
          sslSocket(socket, sslContext) {
    }

    bool handshake() override {
        code = sslSocket.handshake(asio::ssl::stream_base::client, code);
        return code.value() == 0;
    }

    bool shutdown() override {
        code = sslSocket.shutdown(code);
        return code.value() == 0;
    }

    int64_t read(void *buffer, size_t length) override {
        size_t read = sslSocket.read_some(asio::buffer(buffer, length), code);
        if (code) {
            return -1;
        }
        return static_cast<int64_t>(read);
    }

    int64_t write(const void *buffer, size_t length) override {
        size_t wrote = sslSocket.write_some(asio::buffer(buffer, length), code);
        if (code) {
            return -1;
        }
        return static_cast<int64_t>(wrote);
    }

    asio::ssl::context sslContext;
    asio::ssl::stream<asio::ip::tcp::socket &> sslSocket;
};

HttpClient::Ptr HttpClient::create(const std::string &url, const std::string &proxy) {
    bool ssl;
    auto url_result = RequestParser::parse(url);
    IPAddress::Ptr address;
    // GCOVR_EXCL_START
    if (!proxy.empty()) {
        auto proxy_result = RequestParser::parse(proxy);
        if (proxy_result.address == nullptr) {
            return nullptr;
        }
        url_result.request->setUrl(url);
        url_result.request->set("via:", proxy);
        url_result.request->set("proxy-connection", "keep-alive");
        address = std::move(proxy_result.address);
        ssl = strcmpDoNotCase("https", proxy_result.url.getProtocol().c_str());
    }
    // GCOVR_EXCL_STOP
    else {
        if (url_result.address == nullptr) {
            return nullptr;
        }
        address = std::move(url_result.address);
        ssl = strcmpDoNotCase("https", url_result.url.getProtocol().c_str());
    }

    url_result.request->set("user-agent", "sese-httpclient/1.0");
    url_result.request->set("connection", "keep-alive");

    auto result = MAKE_UNIQUE_PRIVATE(HttpClient);
    if (ssl) {
        result->impl = std::make_unique<SSLImpl>(address, std::move(url_result.request));
    } else {
        result->impl = std::make_unique<Impl>(address, std::move(url_result.request));
    }
    return result;
}

HttpClient::~HttpClient() {
}

bool HttpClient::request() const {
    return impl->request();
}

int HttpClient::getLastError() const {
    return impl->getLastError();
}

std::string HttpClient::getLastErrorString() const {
    return impl->getLastErrorString();
}

int64_t HttpClient::read(void *buf, size_t len) const {
    return impl->read(buf, len);
}

int64_t HttpClient::write(const void *buf, size_t len) const {
    return impl->write(buf, len);
}

Request::Ptr &HttpClient::getRequest() const {
    return impl->req;
}

Response::Ptr &HttpClient::getResponse() const {
    return impl->resp;
}

void HttpClient::setReadData(io::PeekableStream *read_data, size_t expect_total) const {
    impl->read_data = read_data;
    impl->expect_total = expect_total;
}

void HttpClient::setWriteData(io::OutputStream *write_data) const {
    impl->write_data = write_data;
}

void HttpClient::setReadCallback(const ReadCallback &read_callback, size_t expect_total) const {
    impl->read_callback = read_callback;
    impl->expect_total = expect_total;
}

void HttpClient::setWriteCallback(const WriteCallback &write_callback) const {
    impl->write_callback = write_callback;
}

} // namespace sese::net::http