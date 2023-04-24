#pragma once

#include "sese/net/V2Server.h"

namespace sese::net::V2::http {

    struct HttpServerOption : public v2::ServerOption {

            void onConnect(v2::IOContext &ctx) noexcept override;

            void onHandle(v2::IOContext &ctx) noexcept override;

            void onClosing(v2::IOContext &tx) noexcept override;
    };

}