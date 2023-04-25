#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/Http2Connection.h"
#include "sese/net/http/Http2FrameInfo.h"

#include <map>

namespace sese::net::v2::http {

    struct [[deprecated("此实现是不完整的")]] Http2ServerOption : public ServerOption {
        Http2ServerOption() = delete;
        explicit Http2ServerOption(size_t handleThreads = 4) noexcept;
        ~Http2ServerOption() noexcept override;

        void onConnect(IOContext &ctx) noexcept override;
        void onHandle(IOContext &ctx) noexcept override;
        void onClosing(IOContext &tx) noexcept override;

        void onHttpHandle(IOContext &ctx) noexcept;
        void onHttp2Handle(IOContext &ctx, net::http::Http2Connection::Ptr conn) noexcept;

        static bool readFrame(IOContext &ctx, sese::net::http::Http2FrameInfo &info) noexcept;
        static void sendGoaway(IOContext &ctx, uint32_t sid, uint32_t eid) noexcept;

    protected:
        sese::ThreadPool threadPool;
        // 对 connMap 操加锁
        std::mutex mutex;
        std::map<socket_t, net::http::Http2Connection::Ptr> connMap;
    };

}// namespace sese::net::v2::http