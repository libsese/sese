// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file GZipFileOutputStream.h
/// \author kaoru
/// \brief GZip 文件输出流（压缩）
/// \version 0.1
/// \date 2023年3月15日

#pragma once

#include "sese/io/OutputStream.h"

namespace sese {

/// GZip 文件输出流（压缩）
class  GZipFileOutputStream : public io::OutputStream {
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

} // namespace sese