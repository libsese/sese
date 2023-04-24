#pragma once

#include "sese/net/http/Http2Stream.h"

#include <map>
#include <mutex>

namespace sese::net::http {

    class Http2Connection {
    public:
        using Ptr = std::shared_ptr<Http2Connection>;
        
        void addStream(uint32_t sid, const Http2Stream::Ptr &stream) noexcept;
        Http2Stream::Ptr find(uint32_t sid);

        /// 此互斥量用于同步发送进度
        std::mutex mutex;
        std::map<uint8_t, Http2Stream::Ptr> streamMap;
    };

}// namespace sese::net::http