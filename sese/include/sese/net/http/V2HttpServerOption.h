#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/HttpServer.h"

namespace sese::net::v2::http {

    using HttpContext = sese::http::HttpServiceContext<sese::net::v2::IOContext>;

    struct HttpServerOption : public ServerOption {
        void onHandle(sese::net::v2::IOContext *ctx) noexcept override;

        virtual void onRequest(HttpContext *ctx) noexcept;
    };
}// namespace sese::net::v2::http