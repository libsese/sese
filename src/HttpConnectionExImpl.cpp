#include <utility>

#include "HttpConnectionEx.h"

HttpConnectionExImpl::HttpConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                                           SharedSocket socket) : HttpConnectionEx(service, context),
                                                                  socket(std::move(socket)) {
}


void HttpConnectionExImpl::writeBlock(const char *buffer, size_t length,
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

void HttpConnectionExImpl::readBlock(char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    async_read(*this->socket, asio::buffer(buffer, length), [conn = shared_from_this(), buffer, length, callback](const asio::error_code &error, std::size_t read) {
        if (error || read == length) {
            callback(error);
        } else {
            conn->readBlock(buffer + read, length - read, callback);
        }
    });
}

HttpsConnectionExImpl::HttpsConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                                             SharedStream stream) : HttpConnectionEx(service, context),
                                                                    stream(std::move(stream)) {
}


void HttpsConnectionExImpl::writeBlock(const char *buffer, size_t length,
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

void HttpsConnectionExImpl::readBlock(char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    this->stream->async_read_some(asio::buffer(buffer, length),
        [conn = getPtr(), buffer, length, callback](const asio::error_code &error, size_t read) {
            if (error || read == length) {
                callback(error);
            } else {
                conn->readBlock(buffer + read, length - read, callback);
            }
        });
}
