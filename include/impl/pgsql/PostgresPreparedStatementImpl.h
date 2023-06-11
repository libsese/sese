#pragma once

#include <sese/db/PreparedStatement.h>
#include <pgsql/PostgresStmtResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API PostgresPreparedStatementImpl : public PreparedStatement {
    public:
        explicit PostgresPreparedStatementImpl(const std::string &stmt, int count, PGconn *conn) noexcept;
        ~PostgresPreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        bool setDouble(uint32_t index, double &value) noexcept override;
        bool setFloat(uint32_t index, float &value) noexcept override;
        bool setInteger(uint32_t index, int32_t &value) noexcept override;
        bool setLong(uint32_t index, int64_t &value) noexcept override;
        bool setText(uint32_t index, const char *value) noexcept override;
        bool setNull(uint32_t index) noexcept override;     //设置空

    protected:
        PGconn *conn;
        std::string stmt;
        int count = 0;
        Oid *paramTypes;
        const char **paramValues;
        std::string *strings;
    };

}// namespace sese::db::impl