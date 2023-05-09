#pragma once

#include "sese/util/InputStream.h"
#include "sese/net/http/Header.h"
#include "sese/net/http/Huffman.h"
#include "sese/net/http/DynamicTable.h"

namespace sese::net::http {
    class API HPackUtil {
    public:
        /// 尝试从流中解析 HPACK 压缩后的 HEADERS
        /// \param src 流来源
        /// \param contentLength 需要解析的内容长度
        /// \param table 解析所用的动态表
        /// \param header 存放解析结果
        /// \return 解析成功与否
        bool decode(InputStream *src, size_t contentLength,DynamicTable &table, Header &header) noexcept;

    protected:
        static int decodeInteger(uint8_t &buf, InputStream *src, uint32_t &dest, uint8_t n) noexcept;

        int decodeString(InputStream *src, std::string &dest) noexcept;

    protected:
        HuffmanDecoder decoder;
        HuffmanEncoder encoder;
    };
}// namespace sese::net::http