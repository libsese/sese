/// \file PostgresDriverInstanceImpl.h
/// \brief PSQL 驱动实现
/// \author kaoru
/// \date 2024年04月10日

#pragma once

#include <sese/db/DriverInstance.h>
#include <sese/internal/db/pgsql/PostgresPreparedStatementImpl.h>

namespace sese::db::impl {

    /// \brief PSQL 驱动实现
    class  PostgresDriverInstanceImpl : public DriverInstance {
    public:
        explicit PostgresDriverInstanceImpl(PGconn *conn) noexcept;
        ~PostgresDriverInstanceImpl() noexcept override;

        ResultSet::Ptr executeQuery(const char *sql) noexcept override;
        int64_t executeUpdate(const char *sql) noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

        [[nodiscard]] bool setAutoCommit(bool enable) noexcept override;
        [[nodiscard]] bool getAutoCommit(bool &status) noexcept override;
        [[nodiscard]] bool begin() noexcept override;
        [[nodiscard]] bool commit() noexcept override;
        [[nodiscard]] bool rollback() noexcept override;

        [[nodiscard]] bool getInsertId(int64_t &id) const noexcept override;

    protected:
        PGconn *conn;
        PGresult *result;
        int error = 0;

        std::random_device rd{};
    };
}// namespace sese::db