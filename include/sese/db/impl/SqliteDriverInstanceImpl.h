#pragma once
#include <sese/db/DriverInstance.h>
#include <sese/db/impl/SqliteResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteDriverInstance : public DriverInstance {
    public:
        using Ptr = std::unique_ptr<SqliteDriverInstance>;

        explicit SqliteDriverInstance(sqlite3 *conn) noexcept;
        ~SqliteDriverInstance() noexcept override;
        ResultSet::Ptr executeQuery(const char *sql) const noexcept override;
        uint64_t executeUpdate(const char *sql) const noexcept override;

    private:
        sqlite3 *conn = nullptr;
    };

}