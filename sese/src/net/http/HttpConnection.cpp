#include <sese/net/http/HttpConnection.h>
#include <sese/net/http/HttpUtil.h>

sese::net::http::HttpConnection::~HttpConnection() noexcept {
    if (file) {
        file->close();
        file = nullptr;
    }
}

void sese::net::http::HttpConnection::doResponse() noexcept {
    net::http::HttpUtil::sendResponse(&buffer2, &resp);
}

int64_t sese::net::http::HttpConnection::read(void *buf, size_t len) {
    return this->buffer1.read(buf, len);
}

int64_t sese::net::http::HttpConnection::write(const void *buf, size_t len) {
    return this->buffer2.write(buf, len);
}