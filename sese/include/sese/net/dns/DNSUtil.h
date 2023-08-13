/// \file DNSUtil.h
/// \brief DNS 帧编码解码工具
/// \date 2023年8月11日
/// \author kaoru

#pragma once

#include <sese/net/dns/FrameHeader.h>
#include <sese/net/dns/Query.h>
#include <sese/net/dns/Answer.h>
#include <sese/util/InputStream.h>
#include <sese/util/OutputStream.h>

#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

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

        /// 将一个或多个 Query 解析成 sese::net::dns::Query
        /// \param qcount 数量
        /// \param input 来源
        /// \param vector 输出
        /// \return 成功与否
        static bool decodeQueries(size_t qcount, InputStream *input, std::vector<Query> &vector) noexcept;

        /// 将一个或多个 Answer 解析成 sese::net::dns::Answer
        /// \param acount 数量
        /// \param raw 原始数据
        /// \param input 来源
        /// \param vector 输出
        /// \return 成功与否
        static bool decodeAnswers(size_t acount, sese::InputStream *input, std::vector<Answer> &vector) noexcept;

        /// 编码一个或多个 Query
        /// \param output 输出
        /// \param vector 来源
        static void encodeQueries(OutputStream *output, std::vector<Query> &vector) noexcept;

        /// 编码一个或多个 Answers
        /// \param output 输出
        /// \param vector 来源
        static void encodeAnswers(OutputStream *output, std::vector<Answer> &vector) noexcept;

        /// 从二进制流中解析域名
        /// \param input 输入
        /// \param domain 域名
        /// \return 成功与否
        static bool decodeDomain(InputStream *input, std::string &domain) noexcept;

    private:
        static void decodeFrameFlagsInfo(const uint8_t buf[2], sese::net::dns::FrameFlagsInfo &info) noexcept;
        static void encodeFrameFlagsInfo(uint8_t buf[2], const FrameFlagsInfo &info) noexcept;
    };
}// namespace sese::net::dns