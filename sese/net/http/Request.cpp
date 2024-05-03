#include <sese/net/http/Request.h>

// GCOVR_EXCL_START

void sese::net::http::Request::swap(sese::net::http::Request &another) noexcept {
    std::swap(type, another.type);
    std::swap(uri, another.uri);
    std::swap(version, another.version);
    std::swap(cookies, another.cookies);
    std::swap(headers, another.headers);
    std::swap(query_args, another.query_args);
    body.swap(another.body);
}

// GCOVR_EXCL_STOP