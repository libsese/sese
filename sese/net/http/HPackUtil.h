/**
* @file HPackUtil.h
* @author kaoru
* @version 0.2
* @brief HTTP HPACK 算法类
* @date 2023年9月13日
*/

#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"
#include "sese/util/NotInstantiable.h"
#include "sese/net/http/Header.h"
#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/Huffman.h"

namespace sese::net::http {

/// HPACK 解压缩工具类
class  HPackUtil final : public NotInstantiable {
public:
    /// 工具指定输入流
    using InputStream = io::InputStream;
    /// 工具指定输出流
    using OutputStream = io::OutputStream;

    HPackUtil() = delete;

    /// 尝试从流中解析 HPACK 压缩后的 HEADERS
    /// \param src 流来源
    /// \param content_length 需要解析的内容长度
    /// \param table 解析所用的动态表
    /// \param header 存放解析结果
    /// \param is_resp 指示当前是否为响应
    /// \return 解析成功返回0，否则返回错误码
    static uint32_t decode(InputStream *src, size_t content_length, DynamicTable &table, Header &header, bool is_resp) noexcept;

    /// 尝试将 HEADERS 按照 HPACK 格式压缩
    /// \param dest 目的流
    /// \param table 压缩所用动态表
    /// \param once_header 非索引字段
    /// \param indexed_header 索引字段
    /// \return 压缩产生的缓存大小
    static size_t encode(OutputStream *dest, DynamicTable &table, Header &once_header, Header &indexed_header) noexcept;

private:
    static int decodeInteger(uint8_t &buf, InputStream *src, uint32_t &dest, uint8_t n) noexcept;

    static int decodeString(InputStream *src, std::string &dest) noexcept;

    static size_t encodeIndexCase0(OutputStream *dest, size_t index) noexcept;

    static size_t encodeIndexCase1(OutputStream *dest, size_t index) noexcept;

    static size_t encodeIndexCase2(OutputStream *dest, size_t index) noexcept;

    static size_t encodeIndexCase3(OutputStream *dest, size_t index) noexcept;

    static size_t encodeString(OutputStream *dest, const std::string &str) noexcept;

    static std::string buildCookieString(const Cookie::Ptr &cookie) noexcept;

    /// 检查伪字段是否重复并设置头部值
    /// @see sese::net::http::HttpConverter
    /// @param header 目标头
    /// @param key 键
    /// @param value 值
    /// @return 是否设置成功
    static bool setHeader(Header &header, const std::string &key, const std::string &value) noexcept;

    /// 校验伪字段
    /// @param header 头部
    /// @param is_resp 是否为响应
    /// @return 校验结果
    static bool verifyHeader(Header &header, bool is_resp) noexcept;

    static const std::string REQ_PSEUDO_HEADER[4];

    static HuffmanDecoder decoder;
    static HuffmanEncoder encoder;
};
} // namespace sese::net::http