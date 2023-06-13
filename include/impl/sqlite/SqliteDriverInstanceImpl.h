#pragma once

#include <sese/db/DriverInstance.h>
#include <sqlite/SqlitePreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteDriverInstanceImpl final : public DriverInstance {
    public:
        explicit SqliteDriverInstanceImpl(sqlite3 *conn) noexcept;
        ~SqliteDriverInstanceImpl() noexcept override;
        ResultSet::Ptr executeQuery(const char *sql) noexcept override;
        int64_t executeUpdate(const char *sql) noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        sqlite3 *conn = nullptr;
    };

}// namespace sese::db::impl