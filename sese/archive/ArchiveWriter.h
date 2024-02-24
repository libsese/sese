/// \file ArchiveWriter.h
/// \brief 存档写入器
/// \author kaoru
/// \date 2024年01月9日


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

#pragma once

#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

#include <filesystem>

namespace sese::archive {

    /// 存档写入器
    class ArchiveWriter {
    public:
        /// 构造写入器
        /// \param output 输出流
        explicit ArchiveWriter(io::OutputStream *output);

        virtual ~ArchiveWriter();

        int setFilterGZip();

        int setFilterBZip2();

        int setFilterLZ4();

        int setFilterLZip();

        int setFilterZstd();

        int setFilterXZ();

        int setFilterLzma();

        int setFilterLzop();

        int setFilterGRZip();

        int setFilterLRZip();

        int setFilterNone();

        int setFormatXar();

        int setFormatTar();

        int setFormatZip();

        int setFormat7z();

        int setFormatISO();

        /// 设置写入存档所使用的密码
        /// \param pwd 密码
        /// \return 成功返回 0
        int setPassword(const std::string &pwd);

        /// 设置存档的 libarchive 选项
        /// \param opt 选项
        /// \return 成功返回 0
        int setOptions(const std::string &opt);

        /// 获取当前错误码
        /// \return 错误码
        int getError();

        /// 获取当前错误信息
        /// \return 错误信息
        const char *getErrorString();

        /// 开始添加实体
        /// \return 操作结果
        bool begin();

        /// 结束添加实体
        /// \return 操作结果
        bool done();

        /// 实体操作函数，添加一个文件或者一个文件夹到当前存档根目录中
        /// \param path 目标路径
        /// \return 是否成功
        bool addPath(const std::filesystem::path &path);

        /// 实体操作函数，添加一个文件或者文件夹到当前存档指定路径中
        /// \param base 存档内路径
        /// \param path 目标路径
        /// \return 是否成功
        bool addPath(const std::filesystem::path &base, const std::filesystem::path &path);

        /// 实体操作函数，不推荐使用
        bool addFile(const std::filesystem::path &file);

        /// 实体操作函数，不推荐使用
        bool addFile(const std::filesystem::path &base, const std::filesystem::path &file);

        /// 实体操作函数，不推荐使用
        bool addDirectory(const std::filesystem::path &dir);

        /// 实体操作函数，不推荐使用
        bool addDirectory(const std::filesystem::path &base, const std::filesystem::path &dir);

        /// 实体操作函数，从流中读取内容并写入到存档指定路径中
        /// \param path 存档内路径
        /// \param input 输入流
        /// \param len 写入大小
        /// \return 是否成功
        bool addStream(const std::filesystem::path &path, io::InputStream *input, size_t len);

        static int openCallback(void *archive, ArchiveWriter *archive_this);

        static int64_t writeCallback(void *archive, ArchiveWriter *archive_this, const void *buffer, size_t len);

        static int closeCallback(void *archive, ArchiveWriter *archive_this);

        static int freeCallback(void *archive, ArchiveWriter *archive_this);

        static const char *passphraseCallback(void *archive, ArchiveWriter *archive_this);

    protected:
        io::OutputStream *output{};
        void *archive{};
    };
}