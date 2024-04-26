#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/Util.h>
#include <sese/record/Marco.h>

using namespace sese::internal::service::http::v3;

HttpConnectionImpl::HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : HttpConnection(service, context) {}

void HttpConnectionImpl::readHeader() {
    asio::async_read_until(this->socket, this->asio_dynamic_buffer, "\r\n", [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (conn->keepalive) {
            conn->keepalive = false;
            conn->timer.cancel();
        }
        if (error) {
            // 出现错误，应该断开连接
            return;
        }
        auto std_input = std::istream{&conn->asio_dynamic_buffer};
        auto stream = io::StdInputStreamWrapper(std_input);
        streamMove(&conn->parse_buffer, &stream, bytes_transferred);
        if (bytes_transferred == 2) {
            SESE_INFO("PARSING HEADER");
            auto parse_status = sese::net::http::HttpUtil::recvRequest(&conn->parse_buffer, &conn->request);
            if (!parse_status) {
                // 解析失败，应该断开连接
                return;
            }
            conn->parse_buffer.freeCapacity();
            conn->expect_length = toInteger(conn->request.get("content-length", "0"));
            conn->real_length = sese::streamMove(&conn->request.getBody(), &stream, conn->expect_length);

            if (conn->real_length < conn->expect_length) {
                conn->readBody();
            } else {
                conn->handleRequest();
            }
        } else {
            conn->readHeader();
        }
    });
}

void HttpConnectionImpl::readBody() {
    asio::async_read(this->socket, this->asio_dynamic_buffer, [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            return;
        }
        auto std_input = std::istream{&conn->asio_dynamic_buffer};
        auto stream = io::StdInputStreamWrapper(std_input);
        streamMove(&conn->request.getBody(), &stream, bytes_transferred);
        conn->real_length += bytes_transferred;
        if (conn->real_length >= conn->expect_length) {
            conn->handleRequest();
        } else {
            conn->readBody();
        }
    });
}

void HttpConnectionImpl::handleRequest() {
    auto std_output = std::ostream{&this->asio_dynamic_buffer};
    auto stream = io::StdOutputStreamWrapper(std_output);
    sese::net::http::HttpUtil::sendResponse(&stream, &this->response);
    this->real_length = 0;
    this->expect_length = this->asio_dynamic_buffer.size();
    this->writeHeader();
}

void HttpConnectionImpl::writeHeader() {
    asio::async_write(this->socket, this->asio_dynamic_buffer, [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            return;
        }
        conn->real_length += bytes_transferred;
        if (conn->real_length >= conn->expect_length) {
            conn->expect_length = conn->response.getBody().getLength();
            conn->real_length = 0;

            if (conn->expect_length != 0) {
                auto std_output = std::ostream{&conn->asio_dynamic_buffer};
                auto stream = io::StdOutputStreamWrapper(std_output);
                streamMove(&stream, &conn->response.getBody(), conn->expect_length);
                conn->writeBody();
            } else {
                conn->checkKeepalive();
            }
        } else {
            conn->writeHeader();
        }
    });
}

void HttpConnectionImpl::writeBody() {
    asio::async_write(this->socket, this->asio_dynamic_buffer, [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            return;
        }
        conn->real_length += bytes_transferred;
        if (conn->expect_length >= conn->real_length) {
            conn->expect_length = 0;
            conn->real_length = 0;
            conn->checkKeepalive();
        } else {
            conn->writeBody();
        }
    });
}

void HttpConnectionImpl::checkKeepalive() {
    if (this->keepalive) {
        SESE_INFO("KEEPALIVE");
        this->reset();
        this->timer.async_wait([conn = shared_from_this()](const asio::error_code &error) {
            if (error.value() == 995) {
                SESE_INFO("CANCEL TIMEOUT");
            } else {
                SESE_INFO("TIMEOUT");
                conn->socket.cancel();
            }
        });
        this->readHeader();
    } else {
        SESE_INFO("CLOSE");
    }
}