#pragma once
#include <sese/db/DriverInstance.h>
#include <sqlite/SqlitePreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteDriverInstanceImpl : public DriverInstance {
    public:
        explicit SqliteDriverInstanceImpl(sqlite3 *conn) noexcept;
        ~SqliteDriverInstanceImpl() noexcept override;
        ResultSet::Ptr executeQuery(const char *sql) const noexcept override;
        int64_t executeUpdate(const char *sql) const noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) const noexcept override;

    protected:
        sqlite3 *conn = nullptr;
    };

}// namespace sese::db::impl