#pragma once

#include "sese/util/InputStream.h"
#include "sese/util/OutputStream.h"
#include "sese/util/NotInstantiable.h"
#include "sese/net/http/Header.h"
#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/Huffman.h"

namespace sese::net::http {
    class API HPackUtil : public NotInstantiable {
    public:
        /// 尝试从流中解析 HPACK 压缩后的 HEADERS
        /// \param src 流来源
        /// \param contentLength 需要解析的内容长度
        /// \param table 解析所用的动态表
        /// \param header 存放解析结果
        /// \return 解析成功与否
        static bool decode(InputStream *src, size_t contentLength, DynamicTable &table, Header &header) noexcept;

        /// 尝试将 HEADERS 按照 HPACK 格式压缩
        /// \param dest 目的流
        /// \param table 压缩所用动态表
        /// \param onceHeader 非索引字段
        /// \param indexedHeader 索引字段
        /// \return 压缩产生的缓存大小
        static size_t encode(OutputStream *dest,DynamicTable &table, Header &onceHeader, Header &indexedHeader) noexcept;

    private:
        static int decodeInteger(uint8_t &buf, InputStream *src, uint32_t &dest, uint8_t n) noexcept;

        static int decodeString(InputStream *src, std::string &dest) noexcept;

        static size_t encodeIndexCase0(OutputStream *dest, size_t index) noexcept;

        static size_t encodeIndexCase1(OutputStream *dest, size_t index) noexcept;

        static size_t encodeIndexCase2(OutputStream *dest, size_t index) noexcept;

        static size_t encodeString(OutputStream *dest, const std::string &str) noexcept;

        static std::string buildCookieString(const Cookie::Ptr& cookie) noexcept;

        static HuffmanDecoder decoder;
        static HuffmanEncoder encoder;
    };
}// namespace sese::net::http