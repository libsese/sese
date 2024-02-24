/// \file PreparedStatement.h
/// \author kaoru
/// \brief 预处理语句对象
/// \version 0.2

#pragma once

#include <sese/db/ResultSet.h>
#include <sese/db/Metadata.h>
#include <sese/text/DateTimeFormatter.h>

namespace sese::db {

    /// \brief 预处理语句对象
    class SESE_DB_API PreparedStatement {
    public:
        using Ptr = std::unique_ptr<PreparedStatement>;

        /// \brief 析构函数
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
        virtual bool setDouble(uint32_t index, const double &value) noexcept = 0;
        /// \brief 设置单精度浮点值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setFloat(uint32_t index, const float &value) noexcept = 0;
        /// \brief 设置整型值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setInteger(uint32_t index, const int32_t &value) noexcept = 0;
        /// \brief 设置长整型值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setLong(uint32_t index, const int64_t &value) noexcept = 0;
        /// \brief 设置文本值
        /// \param index 索引
        /// \param value 值
        /// \return 是否设置成功
        virtual bool setText(uint32_t index, const char *value) noexcept = 0;
        /// \brief 设置为 NULL
        /// \param index 索引
        /// \return 是否设置成功
        virtual bool setNull(uint32_t index) noexcept = 0;
        /// \brief 设置为 dateTime
        /// \param index 索引
        /// \param value 日期
        /// \return 是否设置成功
        virtual bool setDateTime(uint32_t index, const sese::DateTime &value) noexcept = 0;
        /// \brief 获取结果集列类型
        /// \param index 索引值
        /// \param type 列类型
        /// \return 是否获取成功
        virtual bool getColumnType(uint32_t index, MetadataType &type) noexcept = 0;
        /// \brief 获取结果集列大小，此接口通常只在类型为二进制或字符串时有实际作用，且单位是字节
        /// \param index 索引值
        /// \return 失败返回 -1
        virtual int64_t getColumnSize(uint32_t index) noexcept = 0;

        /// 获取实例对应驱动的错误码
        /// \return 错误码
        [[nodiscard]] virtual int getLastError() const noexcept = 0;

        /// 获取实例对应驱动的错误信息，需要判空
        /// \return 错误信息
        [[nodiscard]] virtual const char *getLastErrorMessage() const noexcept = 0;
    };
}// namespace sese::db