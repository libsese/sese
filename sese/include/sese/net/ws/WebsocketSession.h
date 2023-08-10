/// \file WebsocketSession.h
/// \brief Websocket 会话类
/// \author kaoru
/// \date 2023年8月10日
/// \version 0.1

#pragma once

#include <sese/util/ByteBuilder.h>

namespace sese::net::ws {
    /// Websocket 会话类
    struct WebsocketSession {
        /// Websocket 会话文件描述符，通常是负数
        int fd = 0;
        /// 缓存
        ByteBuilder buffer{4096};
        /// 额外数据指针
        void *data = nullptr;
    };
}// namespace sese::net::ws