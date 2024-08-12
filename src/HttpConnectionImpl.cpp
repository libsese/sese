#include "HttpServiceImpl.h"

#include <sese/internal/service/http/HttpServiceImpl_V3.h>

#include <utility>

HttpConnectionImpl::HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                                       SharedSocket socket)
    : HttpConnection(service, context), socket(std::move(socket)) {
}

void HttpConnectionImpl::writeBlock(const char *buffer, size_t length,
                                    const std::function<void(const asio::error_code &code)> &callback) {
    async_write(*this->socket, asio::buffer(buffer, length),
                [conn = shared_from_this(), buffer, length, callback](
            const asio::error_code &error, std::size_t wrote) {
                    if (error || wrote == length) {
                        callback(error);
                    } else {
                        conn->writeBlock(buffer + wrote, length - wrote, callback);
                    }
                });
}

void HttpConnectionImpl::asyncReadSome(const asio::mutable_buffers_1 &buffer,
                                       const std::function<void(const asio::error_code &error,
                                                                std::size_t bytes_transferred)> &callback) {
    this->socket->async_read_some(buffer, callback);
}

void HttpConnectionImpl::checkKeepalive() {
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

HttpsConnectionImpl::HttpsConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                                         SharedStream stream)
    : HttpConnection(service, context), stream(std::move(stream)) {
}

HttpsConnectionImpl::~HttpsConnectionImpl() {
    asio::error_code error = this->stream->shutdown(error);
}

void HttpsConnectionImpl::writeBlock(const char *buffer, size_t length,
                                     const std::function<void(const asio::error_code &code)> &callback) {
    this->stream->async_write_some(asio::buffer(buffer, length),
                                   [conn = getPtr(), buffer, length, callback](
                               const asio::error_code &error, size_t wrote) {
                                       if (error || wrote == length) {
                                           callback(error);
                                       } else {
                                           conn->writeBlock(buffer + wrote, length - wrote, callback);
                                       }
                                   });
}

void HttpsConnectionImpl::asyncReadSome(const asio::mutable_buffers_1 &buffer,
                                        const std::function<void(const asio::error_code &error,
                                                                 std::size_t bytes_transferred)> &callback) {
    this->stream->async_read_some(buffer, callback);
}

void HttpsConnectionImpl::checkKeepalive() {
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
