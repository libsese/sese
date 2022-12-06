/// \file PreparedStatement.h
/// \author kaoru
/// \brief 预处理语句对象
/// \version 0.1
#pragma once
#include <sese/db/ResultSet.h>

namespace sese::db {

    /// \brief 预处理语句对象
    class SESE_DB_API PreparedStatement {
    public:
        using Ptr = std::unique_ptr<PreparedStatement>;

        /// \brief 虚构函数
        virtual ~PreparedStatement() noexcept = default;

        /// \brief 执行查询
        /// \return 查询结果集
        /// \retval nullptr 查询失败
        virtual ResultSet::Ptr executeQuery() noexcept = 0;
        /// \brief 执行更新
        /// \return 统计更新的行数
        /// \retval -1 查询失败
        virtual int64_t executeUpdate() noexcept = 0;

        /// \brief 设置双精度浮点值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setDouble(uint32_t index, double &value) noexcept = 0;
        /// \brief 设置单精度浮点值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setFloat(uint32_t index, float &value) noexcept = 0;
        /// \brief 设置整型值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setInteger(uint32_t index, int32_t &value) noexcept = 0;
        /// \brief 设置长整型值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setLong(uint32_t index, int64_t &value) noexcept = 0;
        /// 设置文本值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setText(uint32_t index, const char *value) noexcept = 0;
        /// \brief 设置为 NULL
        /// \param index 索引
        /// \return 是否设置成功
        virtual bool setNull(uint32_t index) noexcept = 0;
    };
}// namespace sese::db