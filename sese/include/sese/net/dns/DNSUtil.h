/// \file DNSUtil.h
/// \brief DNS 帧编码解码工具
/// \date 2023年8月11日
/// \author kaoru

#pragma once

#include <sese/net/dns/FrameHeader.h>

namespace sese::net::dns {
    /// DNS 帧编码解码工具
    class API DNSUtil {
    public:
        /// 从 12 字节缓存中读取帧头信息
        /// \param buf 12 字节缓存
        /// \param info 帧头信息存放
        static void decodeFrameHeaderInfo(const uint8_t buf[12], sese::net::dns::FrameHeaderInfo &info) noexcept;

        /// 写缓存中写入 12 字节的帧头信息
        /// \param buf 12 字节缓存
        /// \param info 帧头信息来源
        static void encodeFrameHeaderInfo(uint8_t buf[12], const FrameHeaderInfo &info) noexcept;

    private:
        static void decodeFrameFlagsInfo(const uint8_t buf[2], sese::net::dns::FrameFlagsInfo &info) noexcept;
        static void encodeFrameFlagsInfo(uint8_t buf[2], const FrameFlagsInfo &info) noexcept;
    };
}// namespace sese::net::dns