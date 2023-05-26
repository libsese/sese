#pragma once
#include <sese/db/DriverInstance.h>
#include <maria/MariaPreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API MariaDriverInstanceImpl : public DriverInstance {
    public:
        explicit MariaDriverInstanceImpl(MYSQL *conn) noexcept;
        ~MariaDriverInstanceImpl() noexcept override;

        ResultSet::Ptr executeQuery(const char *sql) const noexcept override;
        int64_t executeUpdate(const char *sql) const noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) const noexcept override;

    protected:
        MYSQL *conn;
    };

}// namespace sese::db::impl