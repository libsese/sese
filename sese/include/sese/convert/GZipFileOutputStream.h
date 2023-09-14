/// \file GZipFileOutputStream.h
/// \author kaoru
/// \brief GZip 文件输出流（压缩）
/// \version 0.1
/// \date 2023年3月15日

#pragma once

#include "sese/io/OutputStream.h"

namespace sese {

    /// GZip 文件输出流（压缩）
    class API GZipFileOutputStream : public io::OutputStream {
    public:
        static void deleter(GZipFileOutputStream *data) noexcept;

        using Ptr = std::shared_ptr<GZipFileOutputStream>;

        /// 创建文件输出流对象
        /// \param file 文件的相对或者绝对路径
        /// \param level 压缩等级(取值范围为0~9，自动处理数值区间)
        /// \retval nullptr 打开文件失败
        static GZipFileOutputStream::Ptr create(const char *file, size_t level) noexcept;

        int64_t write(const void *buffer, size_t length) override;

        void close() noexcept;

    private:
        GZipFileOutputStream() = default;

        void *gzFile = nullptr;
    };

}// namespace sese