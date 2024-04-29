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
    service->handleRequest(shared_from_this());
    auto std_output = std::ostream{&this->asio_dynamic_buffer};
    auto stream = io::StdOutputStreamWrapper(std_output);
    sese::net::http::HttpUtil::sendResponse(&stream, &this->response);
    this->real_length = 0;
    this->expect_length = this->asio_dynamic_buffer.size();
    this->writeHeader();
}

void HttpConnectionImpl::writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    asio::async_write(this->socket, asio::buffer(buffer, length), [conn = shared_from_this(), buffer, length, callback](const asio::error_code &code, std::size_t wrote) {
        if (code || wrote == length) {
            callback(code);
        } else {
            conn->writeBlock(buffer + wrote, length - wrote, callback);
        }
    });
}

void HttpConnectionImpl::writeHeader() {
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    auto std_input = std::istream{&this->asio_dynamic_buffer};
    auto stream = io::StdInputStreamWrapper(std_input);
    l = stream.peek(this->send_buffer, l);
    this->real_length += l;
    this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
        if (error) {
            return;
        }
        if (conn->expect_length > conn->real_length) {
            conn->writeHeader();
        } else {
            conn->expect_length = 0;
            conn->real_length = 0;
            if (conn->ranges.size() == 1) {
                // 单区间文件
                conn->expect_length = conn->range_iterator->len;
                conn->file->setSeek(static_cast<int64_t>(conn->range_iterator->begin), io::Seek::BEGIN);
                conn->writeSingleRange();
            } else if (conn->ranges.size() > 1) {
                // 多区间文件
                conn->writeRanges();
            } else if ((conn->expect_length = conn->response.getBody().getReadableSize())) {
                // body 响应
                conn->writeBody();
            } else {
                // keepalive
                conn->checkKeepalive();
            }
        }
    });
}

void HttpConnectionImpl::writeBody() {
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    l = response.getBody().peek(this->send_buffer, l);
    this->real_length += l;
    this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
        if (error) {
            return;
        }
        if (conn->expect_length > conn->real_length) {
            conn->writeBody();
        } else {
            // keepalive
            conn->checkKeepalive();
        }
    });
}

void HttpConnectionImpl::writeSingleRange() {
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    this->real_length += l;
    l = this->file->read(this->send_buffer, l);
    this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
        if (error) {
            return;
        }
        if (conn->expect_length > conn->real_length) {
            conn->writeSingleRange();
        } else {
            // keepalive
            conn->checkKeepalive();
        }
    });
}

void HttpConnectionImpl::writeRanges() {
    if (this->real_length >= this->expect_length) {
        if (this->range_iterator == this->ranges.begin()) {
            // 首个区间
            auto subheader = std::string("--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + this->content_type + "\r\n" +
                             "content-range: " + this->range_iterator->toString(this->filesize) + "\r\n\r\n";
            assert(subheader.length() <= MTU_VALUE);
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->file->setSeek(static_cast<int64_t>(conn->range_iterator->begin), io::Seek::BEGIN);
                conn->expect_length = conn->range_iterator->len;
                conn->real_length = 0;
                conn->writeRanges();
                conn->range_iterator += 1;
            });
            return;
        }
        if (this->range_iterator == this->ranges.end()) {
            // 最后一个区间
            auto subheader = std::string("\r\n--") + HTTPD_BOUNDARY + "--\r\n";
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            assert(subheader.length() <= MTU_VALUE);
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->checkKeepalive();
            });
        } else {
            // 中间的区间
            auto subheader = std::string("\r\n--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + this->content_type + "\r\n" +
                             "content-range: " + this->range_iterator->toString(this->filesize) + "\r\n\r\n";
            assert(subheader.length() <= MTU_VALUE);
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->file->setSeek(static_cast<int64_t>(conn->range_iterator->begin), io::Seek::BEGIN);
                conn->expect_length = conn->range_iterator->len;
                conn->real_length = 0;
                conn->writeRanges();
                conn->range_iterator += 1;
            });
        }
    } else {
        auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
        this->real_length += l;
        l = this->file->read(this->send_buffer, l);
        this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
            if (error) {
                return;
            }
            conn->writeRanges();
        });
    }
}

void HttpConnectionImpl::checkKeepalive() {
    if (this->keepalive) {
        this->reset();
        this->timer.async_wait([conn = shared_from_this()](const asio::error_code &error) {
            if (error.value() == 995) {
                // cancel
            } else {
                conn->socket.cancel();
            }
        });
        this->readHeader();
    } else {

    }
}