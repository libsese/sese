#include <sese/net/http/Request.h>

void sese::net::http::Request::swap(sese::net::http::Request &another) noexcept {
    std::swap(type, another.type);
    std::swap(url, another.url);
    std::swap(version, another.version);
    std::swap(cookies, another.cookies);
    std::swap(headers, another.headers);
    body.swap(another.body);
}