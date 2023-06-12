/// \file DriverManager.h
/// \author kaoru
/// \brief 数据库驱动实例管理器
/// \version 0.1
#pragma once
#include <sese/db/DriverInstance.h>

namespace sese::db {
    /// \brief 数据库类型枚举
    enum class DatabaseType {
        MySql,
        Maria,
        Sqlite,
        Postgres
    };

    /// \brief 数据库驱动实例管理器
    class SESE_DB_API DriverManager {
    public:
        /// \brief 创建数据库驱动实例
        /// \param type 数据库类型
        /// \param connString 连接字符串
        /// \return 数据库驱动实例
        /// \retval nullptr 创建数据库驱动实例失败，且通常是不可恢复错误，但极少出现
        static DriverInstance::Ptr getInstance(DatabaseType type, const char *connString) noexcept;
    };
}// namespace sese::db