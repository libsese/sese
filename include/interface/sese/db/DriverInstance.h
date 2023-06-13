/// \file DriverInstance.h
/// \author kaoru
/// \version 0.1
/// \brief 数据库驱动实例类
#pragma once
#include <sese/db/Config.h>
#include <sese/db/PreparedStatement.h>

namespace sese::db {

    /// \brief 数据库驱动实例类
    class SESE_DB_API DriverInstance {
    public:
        using Ptr = std::unique_ptr<DriverInstance>;

        /// \brief 析构函数
        virtual ~DriverInstance() noexcept = default;
        /// 执行查询
        /// \param sql 查询语句
        /// \return 查询结果集
        /// \retval nullptr 为查询失败
        virtual ResultSet::Ptr executeQuery(const char *sql) noexcept = 0;
        /// 执行更新
        /// \param sql 更新预计
        /// \return 统计更新的行数
        /// \retval -1 执行失败
        virtual int64_t executeUpdate(const char *sql) noexcept = 0;
        /// 创建预处理语句对象
        /// \param sql SQL 语句
        /// \return 预处理语句对象
        /// \retval nullptr 创建失败
        virtual PreparedStatement::Ptr createStatement(const char *sql) noexcept = 0;

        /// 获取实例对应驱动的错误码
        /// \return 错误码
        [[nodiscard]] virtual int getLastError() const noexcept = 0;

        /// 获取实例对应驱动的错误信息，需要判空
        /// \return 错误信息
        [[nodiscard]] virtual const char *getLastErrorMessage() const noexcept = 0;
    };
}// namespace sese::db
