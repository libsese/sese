#pragma once

#include "sese/util/Endian.h"
#include "sese/util/NotInstantiable.h"
#include "sese/io/OutputStream.h"

namespace sese {

    class API MemoryViewer : public NotInstantiable {
    public:
        using OutputStream = sese::io::OutputStream;

        /// \brief 获取内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param size 内存大小
        /// \param 十六进制大小写模式
        static void peer(OutputStream *output, void *position, size_t size, bool isCap = true) noexcept;

        /// \brief 获取 8 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer8(OutputStream *output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 16 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer16(OutputStream *output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 16 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer16(OutputStream *output, void *position, EndianType type, bool isCap = true) noexcept;

        /// \brief 获取 32 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer32(OutputStream *output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 32 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer32(OutputStream *output, void *position, EndianType type, bool isCap = true) noexcept;

        /// \brief 获取 64 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
         static void peer64(OutputStream *output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 64 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer64(OutputStream *output, void *position, EndianType type, bool isCap = true) noexcept;

        static char toChar(unsigned char ch, bool isCap) noexcept;
    };


}// namespace sese