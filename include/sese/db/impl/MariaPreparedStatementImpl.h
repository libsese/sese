#pragma once
#include <sese/db/PreparedStatement.h>
#include <sese/db/impl/MariaStmtResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API MariaPreparedStatementImpl : public PreparedStatement {
    public:
        explicit MariaPreparedStatementImpl(MYSQL_STMT *stmt, size_t count) noexcept;
        ~MariaPreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        int64_t setDouble(uint32_t index, double &value) noexcept override;
        int64_t setInteger(uint32_t index, int64_t &value) noexcept override;
        int64_t setText(uint32_t index, const char *value) noexcept override;
        int64_t setNull(uint32_t index) noexcept override;

    protected:
        MYSQL_STMT *stmt;
        size_t count = 0;
        MYSQL_BIND *param;
    };

}// namespace sese::db