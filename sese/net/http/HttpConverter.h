#pragma once

#include <sese/util/NotInstantiable.h>

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

namespace sese::net::http {

    class HttpConverter final : public NotInstantiable {
    public:
        HttpConverter() = delete;

        static bool convertFromHttp2(Request *request);

        static void convert2Http2(Response *response);
    };

}
