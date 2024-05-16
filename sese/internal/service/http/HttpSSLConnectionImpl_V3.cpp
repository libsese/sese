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

HttpSSLConnectionImpl::~HttpSSLConnectionImpl() {
    SESE_INFO("close");
    if (stream) {
        asio::error_code error = this->stream->shutdown(error);
    }
}

void HttpSSLConnectionImpl::writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) {
    this->stream->async_write_some(asio::buffer(buffer, length), [conn = getPtr(), buffer, length, callback](const asio::error_code &error, size_t wrote) {
        if (error || wrote == length) {
            callback(error);
        } else {
            conn->writeBlock(buffer + wrote, length - wrote, callback);
        }
    });
}

void HttpSSLConnectionImpl::readHeader() {
    auto node = new iocp::IOBufNode(MTU_VALUE);
    this->stream->async_read_some(asio::buffer(node->buffer, MTU_VALUE), [conn = getPtr(), node](const asio::error_code &error, std::size_t bytes_transferred) {
        if (conn->keepalive) {
            conn->keepalive = false;
            conn->timer.cancel();
        }
        if (error) {
            // 出现错误，应该断开连接
            conn->disponse();
            delete node;
            return;
        }
        node->size = bytes_transferred;
        conn->io_buffer.push(node);
        bool recv_status = false;
        bool parse_status = false;
        for (int i = 0; i < bytes_transferred; ++i) {
            if (conn->is0x0a && static_cast<char *>(node->buffer)[i] == '\r') {
                conn->is0x0a = false;
                recv_status = true;
                parse_status = sese::net::http::HttpUtil::recvRequest(&conn->io_buffer, &conn->request);
                break;
            }
            conn->is0x0a = (static_cast<char *>(node->buffer)[i] == '\n');
        }
        if (!recv_status) {
            // 接收不完整，应该继续接收
            conn->readHeader();
            return;
        }
        if (!parse_status) {
            // 解析失败，应该断开连接
            conn->disponse();
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
    auto node = new iocp::IOBufNode(MTU_VALUE);
    this->stream->async_read_some(asio::buffer(node->buffer, MTU_VALUE), [conn = getPtr(), node](const asio::error_code &error, std::size_t bytes_transferred) {
        if (error) {
            // 出现错误，应该断开连接
            conn->disponse();
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
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    l = this->dynamic_buffer.read(this->send_buffer, l);
    this->real_length += l;
    this->writeBlock(this->send_buffer, l, [conn = getPtr()](const asio::error_code &error) {
        if (error) {
            // 出现错误，应该断开连接
            conn->disponse();
            return;
        }
        if (conn->expect_length > conn->real_length) {
            conn->writeHeader();
        } else {
            conn->dynamic_buffer.freeCapacity();
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

void HttpSSLConnectionImpl::writeBody() {
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    l = response.getBody().read(this->send_buffer, l);
    this->real_length += l;
    this->writeBlock(this->send_buffer, l, [conn = getPtr()](const asio::error_code &error) {
        if (error) {
            // 出现错误，应该断开连接
            conn->disponse();
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

void HttpSSLConnectionImpl::checkKeepalive() {
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