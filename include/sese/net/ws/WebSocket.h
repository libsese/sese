#pragma once

#include "sese/net/ws/FrameHeader.h"
#include "sese/net/Socket.h"

namespace sese::net::ws {
    /// \brief 普通 Socket 包装的 WebSocket，仅用于处理协议的发送与部分接受处理
    class API WebSocket : public Socket {
    public:
        explicit WebSocket(Socket &socket) noexcept;

        bool readInfo(FrameHeaderInfo &info) noexcept;
        bool writeInfo(const FrameHeaderInfo &info) noexcept;

        /// 读取反掩码后的数据（通常服务端用）
        /// \param buffer 缓存指针
        /// \param len 缓存大小，不应超过 1024
        /// \return 实际读取字节数
        int64_t readWithMasking(uint32_t maskingKey, void *buffer, size_t len) noexcept;
        /// 发送掩码后的数据（通常客户端用）
        /// \param buffer 缓存指针
        /// \param len 缓存大小，不应超过 1024
        /// \return 实际写入字节数
        int64_t writeWithMasking(uint32_t maskingKey, const void *buffer, size_t len) noexcept;

        /// 发送 ping 帧，通常由服务器发起
        /// \return 发送结果
        bool ping() noexcept;
        /// 发送 pong 帧，通常由客户端响应
        /// \return 发送结果
        bool pong() noexcept;

        /// 携带错误信息主动关闭
        /// \param error 错误信息，为 nullptr 则不发送
        /// \param length 错误信息大小，为 0 则不发送
        /// \return 发送结果
        bool closeWithError(void *error, size_t length) noexcept;
    };
}