/// \file V2HttpServerOption.h
/// \author kaoru
/// \brief Http 服务端选项
/// \version 0.2
/// \date 2023.03.01

#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/HttpServer.h"
#include "sese/util/Timer.h"

namespace sese::net::v2::http {

    using HttpContext = sese::net::http::HttpServiceContext<sese::net::v2::IOContext>;

    /// Http 服务选项
    struct HttpServerOption : public ServerOption {

        explicit HttpServerOption(size_t keepAlive = 0) noexcept;

        ~HttpServerOption() noexcept override;

        void onConnect(IOContext &context) noexcept override;

        void onHandle(IOContext &context) noexcept override;

        void onClosing(IOContext &context) noexcept override;

        virtual void onRequest(HttpContext &ctx) noexcept;

    protected:
        size_t keepAlive = 0;
        Timer::Ptr timer;

        std::mutex mutex;
        std::map<socket_t, TimerTask::Ptr> taskMap;
    };
}// namespace sese::net::v2::http