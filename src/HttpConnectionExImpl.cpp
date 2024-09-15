#include <sese/Log.h>

#include "HttpConnectionEx.h"

HttpConnectionExImpl::HttpConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                                           SharedSocket socket) : HttpConnectionEx(service, context),
                                                                  socket(std::move(socket)) {
}


void HttpConnectionExImpl::writeBlocks(const std::vector<asio::const_buffer> &buffers,
                                      const std::function<void(const asio::error_code &code)> &callback) {
    async_write(*this->socket, buffers, [conn = getPtr(), callback](const asio::error_code &error, size_t bytes) {
        callback(error);
    });
}

void HttpConnectionExImpl::readBlock(char *buffer, size_t length,
                                     const std::function<void(const asio::error_code &code)> &callback) {
    is_read = true;
    async_read(
        *this->socket,
        asio::buffer(buffer, length),
        asio::transfer_at_least(1),
        [conn = shared_from_this(), buffer, length, callback](const asio::error_code &error, std::size_t read) {
            conn->is_read = false;
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


void HttpsConnectionExImpl::writeBlocks(const std::vector<asio::const_buffer> &buffers,
                                      const std::function<void(const asio::error_code &code)> &callback) {
    asio::async_write(*this->stream, buffers, [conn = getPtr(), callback](const asio::error_code &error, size_t) {
        callback(error);
    });
}

void HttpsConnectionExImpl::readBlock(char *buffer, size_t length,
                                      const std::function<void(const asio::error_code &code)> &callback) {
    is_read = true;
    this->stream->async_read_some(asio::buffer(buffer, length),
                                  [conn = getPtr(), buffer, length, callback](
                              const asio::error_code &error, size_t read) {
                                      conn->is_read = false;
                                      if (error || read == length) {
                                          callback(error);
                                      } else {
                                          conn->readBlock(buffer + read, length - read, callback);
                                      }
                                  });
}
