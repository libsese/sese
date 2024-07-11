/// \file DriverInstance.h
/// \author kaoru
/// \version 0.1
/// \brief 数据库驱动实例类
#pragma once
#include <sese/db/Config.h>
#include <sese/db/PreparedStatement.h>

namespace sese::db {

    /// \brief 数据库驱动实例类
    class  DriverInstance {
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
        /// \param sql 更新语句
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
        /// 设置事务自动提交
        /// \warning 因官方不支持 libpq 设置 autocommit，且 autoCommit 属性为全局级，在使用 postgresql 时建议使用 begin() 开启事务
        /// \warning 因官方不支持 sqlite3 设置 autocommit，在使用 sqlite 时建议使用 begin() 开启事务
        /// \param enable 设置自动提交
        /// \retval false 设置失败
        [[nodiscard]] virtual bool setAutoCommit(bool enable) noexcept = 0;

        /// 查看自动提交是否打开
        /// \warning 因官方不支持 libpq 获取 autoCommit,在使用 postgresql 时建议使用命令行获取该值
        /// \retval false 查看失败
        [[nodiscard]] virtual bool getAutoCommit(bool &status) noexcept = 0;

        /// 事务回滚
        /// \retval false 回滚失败
        [[nodiscard]] virtual bool rollback() noexcept = 0;

        /// 事务提交
        /// \retval false 提交失败
        [[nodiscard]] virtual bool commit() noexcept = 0;

        /// 获取自增后的id
        /// \warning 因官方不支持 libpq 获取自增 Id，在使用 postgresql 时建议通过命令行使用 SELECT currval(pg_get_serial_sequence('tabName', 'colName')) 进行自增 Id 的获取;
        /// \retval false 获取失败
        [[nodiscard]] virtual bool getInsertId(int64_t &id) const noexcept = 0;

        /// 事务开启
        /// \return false 开始失败
        [[nodiscard]] virtual bool begin() noexcept = 0;
    };
}// namespace sese::db
