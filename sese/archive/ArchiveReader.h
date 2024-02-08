/// \file ArchiveReader.h
/// \brief 存档读取器
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

#include <sese/archive/Config.h>
#include <sese/io/InputStream.h>

#include <functional>
#include <filesystem>

namespace sese::archive {

    /// 存档读取器
    class ArchiveReader{
    public:
        /// 解压回调函数
        using ExtractCallback = std::function<bool(const std::filesystem::path &base_path, Config::EntryType type, io::InputStream *input, size_t readable)>;

        /// 将存档解压至指定位置
        /// \param src_path 存档路径
        /// \param dest_path 解压目的路径
        /// \param pwd 密码，可选的
        /// \return 是否成功
        static bool extract(const std::filesystem::path &src_path, const std::filesystem::path &dest_path, const std::string &pwd = {});

        /// 从输入流中读取存档
        /// \param input 输入流
        explicit ArchiveReader(io::InputStream *input);

        virtual ~ArchiveReader();

        /// 为当前读取的文档设置使用的密码
        /// \param pwd 欲使用的密码
        /// \return 成功返回 0
        int setPassword(const std::string &pwd);

        /// 为当前的存档设置 libarchive 选项
        /// \param opt 选项字符串
        /// \return 成功返回 0
        int setOptions(const std::string &opt);

        /// 执行解压操作
        /// \param callback 解压回调函数
        /// \return 解压是否成功
        bool extract(const ExtractCallback &callback);

        /// 获取当前错误码
        /// \return 错误码
        int getError();

        /// 获取当前错误字符串
        /// \return 错误字符串
        const char *getErrorString();

        static int openCallback(void *a, ArchiveReader *_this);

        static int64_t readCallback(void *a, ArchiveReader *_this, const void **buffer);

        static int closeCallback(void *a, ArchiveReader * _this);

    protected:
        char buffer[4096]{};
        void *archive{};
        io::InputStream *input{};
    };
}