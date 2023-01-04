#pragma once

#include "sese/net/ws/FrameHeader.h"
#include "sese/util/Stream.h"

#include <functional>

namespace sese::net::ws {

    /// \brief 将普通流装饰为 WebSocket，仅用于处理协议的发送与部分接受处理
    class API WebSocketContext : public Stream {
    public:
        explicit WebSocketContext(Stream *stream) noexcept;

        /// \brief 读取内容
        /// \warning 此方法使用的递归，请注意 length 大小
        /// \param buffer 缓存
        /// \param length 需要读取的长度
        /// \return 实际读取长度
        int64_t read(void *buffer, size_t length) override;
        /// \brief 写入内容
        /// \warning 此方法每调用一次都会至少产生一帧
        /// \param buffer 缓存
        /// \param length 需要写入的长度
        /// \return 实际写入的长度
        int64_t write(const void *buffer, size_t length) override;
        void close() override;

    public:
        bool readInfo() noexcept;
        bool writeInfo(const FrameHeaderInfo &info) noexcept;

        /// 直接写入裸流，需要搭配 writeInfo 使用，可手动处理每一帧的大小
        /// \param buffer 缓存指针
        /// \param length 缓存大小
        /// \return 实际写入字节数
        int64_t writeRaw(const void *buffer, size_t length) noexcept;

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

        /// 主动关闭
        /// \return 发送结果
        bool closeNoError() noexcept;
        /// 携带错误信息主动关闭
        /// \param error 错误信息，为 nullptr 则不发送
        /// \param length 错误信息大小，为 0 则不发送
        /// \return 发送结果
        bool closeWithError(const void *error, size_t length) noexcept;
        /// 携带错误信息主动关闭
        /// \param error 错误信息，为 nullptr 则不发送
        /// \param length 错误信息大小，为 0 则不发送
        /// \param maskingKey 使用的掩码
        /// \return 发送结果
        bool closeWithError(const void *error, size_t length, uint32_t maskingKey) noexcept;

    private:
        std::function<int64_t(void *, size_t)> autoRead;// 自动处理掩码的读取
        Stream *stream = nullptr;                       // 被装饰的流
        FrameHeaderInfo info;                           // 当前读取的帧头
        uint64_t readed = 0;                            // 当前帧已读的长度
    };
}// namespace sese::net::ws