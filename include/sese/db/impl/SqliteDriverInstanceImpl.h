#pragma once
#include <sese/db/DriverInstance.h>
#include <sese/db/impl/SqlitePreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteDriverInstance : public DriverInstance {
    public:
        explicit SqliteDriverInstance(sqlite3 *conn) noexcept;
        ~SqliteDriverInstance() noexcept override;
        ResultSet::Ptr executeQuery(const char *sql) const noexcept override;
        int64_t executeUpdate(const char *sql) const noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) const noexcept override;

    private:
        sqlite3 *conn = nullptr;
    };

}