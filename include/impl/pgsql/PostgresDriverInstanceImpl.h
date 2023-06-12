#pragma once
#include <sese/db/DriverInstance.h>
#include <pgsql/PostgresPreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API PostgresDriverInstanceImpl : public DriverInstance {
    public:
        explicit PostgresDriverInstanceImpl(PGconn *conn) noexcept;
        ~PostgresDriverInstanceImpl() noexcept override;

        ResultSet::Ptr executeQuery(const char *sql) noexcept override;
        int64_t executeUpdate(const char *sql) noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        PGconn *conn;
        PGresult *result;
        int error = 0;
    };
}// namespace sese::db