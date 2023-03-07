/// \file WebSocketContext.h
/// \brief WebSocket 包装类
/// \author kaoru
/// \date 2023年3月7日
/// \version 0.2

#pragma once

#include "sese/net/ws/FrameHeader.h"
#include "sese/util/Stream.h"

#include <functional>

namespace sese::net::ws {

    /// WebSocket 包装类
    class API WebSocketContext {
    public:
        explicit WebSocketContext(Stream *stream);

        /// 读取帧头
        /// \param info 帧信息存放处
        /// \return 读取是否成功
        bool readFrameInfo(FrameHeaderInfo &info);

        /// 发送帧头
        /// \param info 发送的帧信息
        /// \return 发送是否成功
        bool writeFrameInfo(const FrameHeaderInfo &info);

        /// 发送 ping 帧
        /// \return 发送是否成功
        bool ping();

        /// 发送 pong 帧
        /// \return 发送是否成功
        bool pong();

        /// 以无掩码的形式读取二进制流至缓存
        /// \param buf 缓存
        /// \param len 缓存大小
        /// \return 实际读取字节数
        int64_t readBinary(void *buf, size_t len);

        /// 以有掩码的形式读取二进制流至缓存
        /// \param buf 缓存大小
        /// \param len 缓存大小
        /// \param maskingKey 掩码
        /// \param offset 指示负载流所在帧的具体位置
        /// \return 实际读取字节数
        int64_t readBinary(void *buf, size_t len, uint32_t maskingKey, size_t &offset);

        /// 以无掩码的形式将缓存写入二进制流
        /// \param buf 缓存
        /// \param len 缓存大小
        /// \return 实际写入字节数
        int64_t writeBinary(const void *buf, size_t len);

        /// 以有掩码的形式将缓存写入二进制流
        /// \param buf 缓存
        /// \param len 缓存大小
        /// \param maskingKey 掩码
        /// \param offset 指示负载流所在帧的具体位置
        /// \return 实际写入字节数
        int64_t writeBinary(const void *buf, size_t len, uint32_t maskingKey, size_t &offset);

        /// 发送关闭且无错误的帧
        /// \return 发送成功
        bool closeNoError();

        /// 发送关闭、有错误且无掩码的帧
        /// \param err 错误信息
        /// \param len 信息长度
        /// \return 发送是否成功
        bool closeWithError(const void *err, size_t len);

        /// 发送关闭、有错误且有掩码的帧
        /// \param err 错误信息
        /// \param len 信息长度
        /// \param maskingKey 掩码
        /// \return 发送是否成功
        bool closeWithError(const void *err, size_t len, uint32_t maskingKey);

    protected:
        Stream *stream = nullptr;
    };

}// namespace sese::net::ws