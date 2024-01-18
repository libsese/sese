#pragma once

#include <sese/util/Util.h>
#include <sese/db/PreparedStatement.h>
#include <sqlite/SqliteStmtResultSetImpl.h>

#include <set>

namespace sese::db::impl {

    class SESE_DB_API SqlitePreparedStatementImpl final : public PreparedStatement {
    public:
        explicit SqlitePreparedStatementImpl(sqlite3_stmt *stmt) noexcept;
        ~SqlitePreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;
        bool setDouble(uint32_t index, double &value) noexcept override;
        bool setFloat(uint32_t index, float &value) noexcept override;
        bool setLong(uint32_t index, int64_t &value) noexcept override;
        bool setInteger(uint32_t index, int32_t &value) noexcept override;
        bool setText(uint32_t index, const char *value) noexcept override;
        bool setNull(uint32_t index) noexcept override;

        bool getColumnType(uint32_t index, MetadataType &type) noexcept override;
        int64_t getColumnSize(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        sqlite3_stmt *stmt;
        bool stmtStatus = false;

        static std::string splitBefore(const std::string &str);
        static const char *IntegerAffinitySet[];
        static const char *TextAffinitySet[];
        static const char *RealAffinitySet[];
    };

}// namespace sese::db::impl