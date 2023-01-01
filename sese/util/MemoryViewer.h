#pragma once
#include "Endian.h"
#include "OutputStream.h"
#include "NotInstantiable.h"

namespace sese {

    class API MemoryViewer : public NotInstantiable {
    public:
        /// \brief 获取内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param size 内存大小
        /// \param 十六进制大小写模式
        static void peer(const OutputStream::Ptr &output, void *position, size_t size, bool isCap = true) noexcept;

        /// \brief 获取 8 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer8(const OutputStream::Ptr &output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 16 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer16(const OutputStream::Ptr &output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 16 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer16(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap = true) noexcept;

        /// \brief 获取 32 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
        static void peer32(const OutputStream::Ptr &output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 32 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer32(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap = true) noexcept;

        /// \brief 获取 64 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param 十六进制大小写模式
         static void peer64(const OutputStream::Ptr &output, void *position, bool isCap = true) noexcept;

        /// \brief 获取 64 bits 内存视图
        /// \param output 输出流
        /// \param position 内存地址
        /// \param type 输出时的 endian 格式
        /// \param 十六进制大小写模式
        static void peer64(const OutputStream::Ptr &output, void *position, EndianType type, bool isCap = true) noexcept;

        static char toChar(unsigned char ch, bool isCap) noexcept;
    };


}// namespace sese