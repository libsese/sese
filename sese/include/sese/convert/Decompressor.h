/**
 * \file Decompressor.h
 * \brief 解压器类
 * \version 0.1
 * \date 2023年3月4日
 * \author kaoru
 */
#pragma once

#include "sese/convert/ZlibConfig.h"
#include "sese/io/OutputStream.h"

namespace sese {

    /// 解压器
    class API Decompressor {
    public:
        using OutputStream = sese::io::OutputStream;

        /// 创建一个解压器
        /// \param type 压缩格式
        /// \param bufferSize 设置内部缓存大小
        explicit Decompressor(CompressionType type, size_t bufferSize = ZLIB_CHUNK_SIZE);

        /// 释放资源
        virtual ~Decompressor();

        /// 设置欲解压的缓存
        /// \param input 欲解压的缓存
        /// \param inputSize 此缓存的大小
        void input(const void *input, unsigned int inputSize);

        /// 进行解压
        /// \param out 解压后的压缩流
        /// \retval Z_OK(0) 当前缓存块已压缩完成
        /// \retval Z_STREAM_ERROR(-2) 其他错误
        /// \retval Z_BUF_ERROR(-5) 输出流容量不足
        int inflate(OutputStream *out);

        /// 重置 z_stream 对象
        int reset();

        /// 当前已处理输入缓存大小
        /// \return 缓存大小
        [[nodiscard]] size_t getTotalIn() const;

        /// 当前已处理输出缓存大小
        /// \return 缓存大小
        [[nodiscard]] size_t getTotalOut() const;

    private:
        void *stream;
        /// 表示 buffer 分配空间大小
        size_t cap = 0;
        /// 表示 buffer 填充大小
        size_t length = 0;
        /// 表示 buffer 已读部分
        size_t read = 0;
        /// 内部缓存 buffer
        unsigned char *buffer;
    };
}// namespace sese