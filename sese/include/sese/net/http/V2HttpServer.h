/// \file V2HttpServer.h
/// \author kaoru
/// \brief Http 服务端选项
/// \version 0.2
/// \date 2023.04.29

#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/HttpServer.h"
#include "sese/util/Timer.h"

namespace sese::net::v2::http {

    using HttpContext = sese::net::http::HttpServiceContext<sese::net::v2::IOContext>;

    /// Http 服务选项
    struct SESE_DEPRECATED HttpServer : public Server {

        ~HttpServer() noexcept override;

        bool init() noexcept override;

        void onConnect(IOContext &context) noexcept override;

        void onHandle(IOContext &context) noexcept override;

        void onClosing(IOContext &context) noexcept override;

        virtual void onRequest(HttpContext &ctx) noexcept;

        void setKeepAlive(size_t seconds) noexcept { keepAlive = seconds; }

    protected:
        size_t keepAlive = 0;
        Timer::Ptr timer;

        std::mutex mutex;
        std::map<socket_t, TimerTask::Ptr> taskMap;
    };
}// namespace sese::net::v2::http