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

/// \file DriverManager.h
/// \author kaoru
/// \brief 数据库驱动实例管理器
/// \version 0.1
#pragma once
#include <sese/db/DriverInstance.h>

namespace sese::db {
    /// \brief 数据库类型枚举
    enum class DatabaseType {
        MY_SQL,
        MARIA,
        SQLITE,
        POSTGRES
    };

    /// \brief 数据库驱动实例管理器
    class  DriverManager {
    public:
        /// \brief 创建数据库驱动实例
        /// \param type 数据库类型
        /// \param conn_string 连接字符串
        /// \return 数据库驱动实例
        /// \retval nullptr 创建数据库驱动实例失败，且通常是不可恢复错误，但极少出现
        static DriverInstance::Ptr getInstance(DatabaseType type, const char *conn_string) noexcept;
    };
}// namespace sese::db