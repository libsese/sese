#pragma once
#include <sese/db/DriverInstance.h>
#include <pgsql/PostgresPreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API PostgresDriverInstanceImpl : public DriverInstance {
    public:
        explicit PostgresDriverInstanceImpl(PGconn *conn) noexcept;
        ~PostgresDriverInstanceImpl() noexcept override;

        ResultSet::Ptr executeQuery(const char *sql) const noexcept override;
        int64_t executeUpdate(const char *sql) const noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) const noexcept override;

    protected:
        PGconn *conn;
    };
}// namespace sese::db