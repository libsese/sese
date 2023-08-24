#pragma once

#include <sese/db/DriverInstance.h>
#include <maria/MariaPreparedStatementImpl.h>

namespace sese::db::impl {

    class SESE_DB_API MariaDriverInstanceImpl final : public DriverInstance {
    public:
        explicit MariaDriverInstanceImpl(MYSQL *conn) noexcept;
        ~MariaDriverInstanceImpl() noexcept override;

        ResultSet::Ptr executeQuery(const char *sql) noexcept override;
        int64_t executeUpdate(const char *sql) noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

        [[nodiscard]] bool setAutoCommit(bool enable) noexcept override;
        [[nodiscard]] bool getAutoCommit() noexcept  override;
        [[nodiscard]] bool commit() noexcept override;
        [[nodiscard]] bool rollback() noexcept override;

        [[nodiscard]] bool getInsertId(int64_t &id) const noexcept override;

    protected:
        MYSQL *conn;
    };

}// namespace sese::db::impl