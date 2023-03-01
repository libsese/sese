/// \file V2HttpServerOption.h
/// \author kaoru
/// \brief Http 服务端选项
/// \version 0.2
/// \date 2023.03.01

#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/HttpServer.h"

namespace sese::net::v2::http {

    using HttpContext = sese::http::HttpServiceContext<sese::net::v2::IOContext>;

    /// Http 服务选项
    struct HttpServerOption : public ServerOption {
        void onHandle(sese::net::v2::IOContext *ctx) noexcept override;

        virtual void onRequest(HttpContext *ctx) noexcept;
    };
}// namespace sese::net::v2::http