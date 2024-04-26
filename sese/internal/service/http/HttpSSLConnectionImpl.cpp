#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/Util.h>
#include <sese/record/Marco.h>

using namespace sese::internal::service::http::v3;

HttpSSLConnectionImpl::HttpSSLConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : HttpConnection(service, context) {}

void HttpSSLConnectionImpl::readHeader() {
    auto node = new iocp::IOBufNode(1024);
    this->stream->async_read_some(asio::buffer(node->buffer, 1024), [conn = shared_from_this(), node](const asio::error_code &error, std::size_t bytes_transferred) {
        if (conn->keepalive) {
            conn->keepalive = false;
            conn->timer.cancel();
        }
        if (error) {
            // 出现错误，应该断开连接
            asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            delete node;
            return;
        }
        node->size = bytes_transferred;
        conn->io_buffer.push(node);
        bool recv_status = false;
        bool parse_status = false;
        for (int i = 0; i < bytes_transferred; ++i) {
            if (conn->is0x0d && (static_cast<char *>(node->buffer)[i] == '\n')) {
                conn->is0x0d = false;
                recv_status = true;
                SESE_INFO("PARSING HEADER");
                parse_status = sese::net::http::HttpUtil::recvRequest(&conn->io_buffer, &conn->request);
                break;
            }
            conn->is0x0d = (static_cast<char *>(node->buffer)[i] == '\r');
        }
        if (!recv_status) {
            // 接收不完整，应该继续接收
            conn->readHeader();
            return;
        }
        if (!parse_status) {
            // 解析失败，应该断开连接
            asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            return;
        }

        conn->expect_length = toInteger(conn->request.get("content-length", "0"));
        conn->real_length = node->getReadableSize();
        if (conn->real_length) {
            // 部分 body
            streamMove(&conn->request.getBody(), &conn->io_buffer, conn->real_length);
        }
        conn->io_buffer.clear();
        if (conn->expect_length != conn->real_length) {
            conn->readBody();
        } else {
            conn->handleRequest();
        }
    });
}

void HttpSSLConnectionImpl::readBody() {
    auto node = new iocp::IOBufNode(1024);
    this->stream->async_read_some(asio::buffer(node->buffer, 1024), [conn = shared_from_this(), node](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            delete node;
            return;
        }
        node->size = bytes_transferred;
        conn->io_buffer.push(node);
        conn->real_length += node->size;
        streamMove(&conn->request.getBody(), &conn->io_buffer, node->size);
        if (conn->real_length >= conn->expect_length) {
            // 理论上 real_length 不可能大于 expect_length，此处预防万一
            conn->io_buffer.clear();
            conn->handleRequest();
        } else {
            conn->readBody();
        }
    });
}

void HttpSSLConnectionImpl::handleRequest() {
    this->service->handleRequest(shared_from_this());
    sese::net::http::HttpUtil::sendResponse(&this->dynamic_buffer, &this->response);
    this->real_length = 0;
    this->expect_length = this->dynamic_buffer.getReadableSize();
    this->writeHeader();
}

void HttpSSLConnectionImpl::writeHeader() {
    auto size = this->dynamic_buffer.peek(this->send_buffer, MTU_VALUE);
    this->stream->async_write_some(asio::buffer(this->send_buffer, size), [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            return;
        }
        conn->dynamic_buffer.trunc(bytes_transferred);
        conn->real_length += bytes_transferred;
        if (conn->real_length >= conn->expect_length) {
            conn->dynamic_buffer.freeCapacity();
            conn->expect_length = conn->response.getBody().getLength();
            conn->real_length = 0;

            if (conn->expect_length != 0) {
                conn->writeBody();
            } else {
                conn->checkKeepalive();
            }
        } else {
            conn->writeHeader();
        }
    });
}

void HttpSSLConnectionImpl::writeBody() {
    auto size = this->dynamic_buffer.peek(this->send_buffer, MTU_VALUE);
    this->stream->async_write_some(asio::buffer(this->send_buffer, size), [conn = shared_from_this()](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            return;
        }
        conn->dynamic_buffer.trunc(bytes_transferred);
        conn->real_length += bytes_transferred;
        if (conn->real_length >= conn->expect_length) {
            conn->dynamic_buffer.freeCapacity();
            conn->checkKeepalive();
        } else {
            conn->writeHeader();
        }
    });
}

void HttpSSLConnectionImpl::checkKeepalive() {
    if (this->keepalive) {
        SESE_INFO("KEEPALIVE");
        this->reset();
        this->timer.expires_from_now(asio::chrono::seconds{this->service->getKeepalive()});
        this->timer.async_wait([conn = shared_from_this()](const asio::error_code &error) {
            if (error.value() == 995) {
                SESE_INFO("CANCEL TIMEOUT");
            } else {
                SESE_INFO("TIMEOUT");
                conn->socket.cancel();
                asio::error_code shutdown_error = conn->stream->shutdown(shutdown_error);
            }
        });
        this->readHeader();
    } else {
        asio::error_code error = this->stream->shutdown(error);
        SESE_INFO("CLOSE");
    }
}