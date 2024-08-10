#include "HttpServiceImpl.h"

#include <sese/internal/service/http/HttpServiceImpl_V3.h>

HttpsConnection::HttpsConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : HttpConnection(service, context) {}

HttpsConnection::~HttpsConnection() {
    if (stream) {
        asio::error_code error = this->stream->shutdown(error);
    }
}

void HttpsConnection::writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    this->stream->async_write_some(asio::buffer(buffer, length), [conn = getPtr(), buffer, length, callback](const asio::error_code &error, size_t wrote) {
        if (error || wrote == length) {
            callback(error);
        } else {
            conn->writeBlock(buffer + wrote, length - wrote, callback);
        }
    });
}

void HttpsConnection::asyncReadSome(const asio::mutable_buffers_1 &buffer, const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &callback) {
    this->stream->async_read_some(buffer, callback);
}

void HttpsConnection::checkKeepalive() {
    if (this->keepalive) {
        this->reset();
        this->timer.async_wait([conn = getPtr()](const asio::error_code &error) {
            if (error == asio::error::operation_aborted) {
            } else {
                conn->socket.cancel();
                conn->disponse();
            }
        });
        this->readHeader();
    } else {
        this->disponse();
    }
}