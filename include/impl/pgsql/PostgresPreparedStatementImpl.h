#pragma once

#include <sese/db/PreparedStatement.h>
#include <pgsql/PostgresResultSetImpl.h>
#include <sstream>
#include <random>

namespace sese::db::impl {

    class SESE_DB_API PostgresPreparedStatementImpl : public PreparedStatement {
    public:
        explicit PostgresPreparedStatementImpl(const std::string &stmt, uint32_t count, PGconn *conn) noexcept;
        ~PostgresPreparedStatementImpl() noexcept override;

        bool createStmt() noexcept;
        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        bool setDouble(uint32_t index, double &value) noexcept override;
        bool setFloat(uint32_t index, float &value) noexcept override;
        bool setInteger(uint32_t index, int32_t &value) noexcept override;
        bool setLong(uint32_t index, int64_t &value) noexcept override;
        bool setText(uint32_t index, const char *value) noexcept override;
        bool setNull(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        PGconn *conn;
        std::string stmt;
        uint32_t count = 0;
        Oid *paramTypes;
        const char **paramValues;
        std::string *strings;
        std::string stmtName;
        bool stmtStatus;
        int error = 0;
        PGresult *result;
    };

}// namespace sese::db::impl