/// \file SqlitePreparedStatementImpl.h
/// \brief SQLite 预处理语句实现
/// \author kaoru
/// \date 2024年04月10日

#pragma once

#include <sese/util/Util.h>
#include <sese/db/PreparedStatement.h>
#include <sese/internal/db/sqlite/SqliteStmtResultSetImpl.h>

#include <set>

namespace sese::db::impl {

    /// \brief SQLite 预处理语句实现
    class  SqlitePreparedStatementImpl final : public PreparedStatement {
    public:
        explicit SqlitePreparedStatementImpl(sqlite3_stmt *stmt, size_t count) noexcept;
        ~SqlitePreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;
        bool setDouble(uint32_t index, const double &value) noexcept override;
        bool setFloat(uint32_t index, const float &value) noexcept override;
        bool setLong(uint32_t index, const int64_t &value) noexcept override;
        bool setInteger(uint32_t index, const int32_t &value) noexcept override;
        bool setText(uint32_t index, const char *value) noexcept override;
        bool setNull(uint32_t index) noexcept override;
        bool setDateTime(uint32_t index, const sese::DateTime &value) noexcept override;

        bool getColumnType(uint32_t index, MetadataType &type) noexcept override;
        int64_t getColumnSize(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        sqlite3_stmt *stmt;
        bool stmtStatus = false;
        bool *isManual;
        size_t count = 0;
        void **buffer;

        static std::string splitBefore(const std::string &str);
        static const char *INTEGER_AFFINITY_SET[];
        static const char *TEXT_AFFINITY_SET[];
        static const char *REAL_AFFINITY_SET[];
    };

}// namespace sese::db::impl