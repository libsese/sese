#pragma once

#include <sese/db/PreparedStatement.h>
#include <pgsql/PostgresResultSetImpl.h>
#include <sstream>
#include <random>

namespace sese::db::impl {

    class SESE_DB_API PostgresPreparedStatementImpl : public PreparedStatement {
    public:
        explicit PostgresPreparedStatementImpl(const std::string &stmtName, const std::string &stmtString, uint32_t count, PGconn *conn) noexcept;
        ~PostgresPreparedStatementImpl() noexcept override;


        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        [[nodiscard]] bool setDouble(uint32_t index, double &value) noexcept override;
        [[nodiscard]] bool setFloat(uint32_t index, float &value) noexcept override;
        [[nodiscard]] bool setInteger(uint32_t index, int32_t &value) noexcept override;
        [[nodiscard]] bool setLong(uint32_t index, int64_t &value) noexcept override;
        [[nodiscard]] bool setText(uint32_t index, const char *value) noexcept override;
        [[nodiscard]] bool setNull(uint32_t index) noexcept override;

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