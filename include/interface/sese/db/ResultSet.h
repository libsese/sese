/// \file ResultSet.h
/// \brief 查询结果集
/// \version 0.1
/// \author kaoru
#pragma once
#include <sese/db/Config.h>
#include <string>

namespace sese::db {

    /// \brief 查询结果集
    class SESE_DB_API ResultSet {
    public:
        using Ptr = std::unique_ptr<ResultSet>;

        /// \brief 析构函数
        virtual ~ResultSet() noexcept = default;
        /// \brief 定位首条查询记录
        virtual void reset() noexcept = 0;
        /// \brief 定位到下一条记录
        /// \return 是否存在下一条记录
        [[nodiscard]] virtual bool next() noexcept = 0;
        // [[nodiscard]] virtual const char *getColumnByIndex(size_t index) const noexcept = 0;
        /// \brief 获取记录列数
        /// \return 结果集记录的列数
        [[nodiscard]] virtual size_t getColumns() const noexcept = 0;

        /// \brief 在当前记录中获取一个整型值
        /// \param index 索引
        /// \return 整型值
        [[nodiscard]] virtual int32_t getInteger(size_t index) const noexcept = 0;
        /// \brief 在当前记录中获取一个长整型值
        /// \param index 索引
        /// \return 长整型值
        [[nodiscard]] virtual int64_t getLong(size_t index) const noexcept = 0;
        /// \brief 在当前记录中获取一个字符串
        /// \param index 索引
        /// \return 字符串
        [[nodiscard]] virtual std::string_view getString(size_t index) const noexcept = 0;
        /// \brief 在当前记录中获取一个双精度浮点值
        /// \param index 索引
        /// \return 双精度浮点值
        [[nodiscard]] virtual double getDouble(size_t index) const noexcept = 0;
        /// \brief 在当前记录中获取一个单精度浮点值
        /// \param index 索引
        /// \return 单精度浮点值
        [[nodiscard]] virtual float getFloat(size_t index) const noexcept = 0;
    };
}