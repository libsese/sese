#include <sese/net/http/Response.h>

// GCOVR_EXCL_START

void sese::net::http::Response::swap(sese::net::http::Response &another) noexcept {
    std::swap(version, another.version);
    std::swap(statusCode, another.statusCode);
    std::swap(cookies, another.cookies);
    std::swap(headers, another.headers);
    body.swap(another.body);
}

// GCOVR_EXCL_STOP