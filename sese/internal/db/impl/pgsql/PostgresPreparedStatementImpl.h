/// \file PostgresPreparedStatementImpl.h
/// \brief PSQL 预处理语句实现
/// \author kaoru
/// \date 2024年04月10日

#pragma once

#include <sese/db/PreparedStatement.h>
#include <sese/internal/db/impl/pgsql/PostgresResultSetImpl.h>
#include <random>
#include <libpq-fe.h>

namespace sese::db::impl {

    /// \brief PSQL 预处理语句实现
    class SESE_DB_API PostgresPreparedStatementImpl : public PreparedStatement {
    public:
        explicit PostgresPreparedStatementImpl(std::string stmt_name, std::string stmt_string, uint32_t count, PGconn *conn) noexcept;
        ~PostgresPreparedStatementImpl() noexcept override;


        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        [[nodiscard]] bool setDouble(uint32_t index, const double &value) noexcept override;
        [[nodiscard]] bool setFloat(uint32_t index, const float &value) noexcept override;
        [[nodiscard]] bool setInteger(uint32_t index, const int32_t &value) noexcept override;
        [[nodiscard]] bool setLong(uint32_t index, const int64_t &value) noexcept override;
        [[nodiscard]] bool setText(uint32_t index, const char *value) noexcept override;
        [[nodiscard]] bool setNull(uint32_t index) noexcept override;
        [[nodiscard]] bool setDateTime(uint32_t index, const sese::DateTime &value) noexcept override;


        [[nodiscard]] bool getColumnType(uint32_t index, MetadataType &type) noexcept override;
        [[nodiscard]] int64_t getColumnSize(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        PGconn *conn;
        std::string stmtName;
        std::string stmtString;
        uint32_t count;
        Oid *paramTypes;
        const char **paramValues;
        std::string *strings;
        int error = 0;
        PGresult *result;
    };

}// namespace sese::db::impl