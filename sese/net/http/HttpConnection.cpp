#include <sese/net/http/HttpConnection.h>
#include <sese/net/http/HttpUtil.h>

sese::net::http::HttpConnection::~HttpConnection() noexcept {
    if (file) {
        file->close();
        file = nullptr;
    }
}