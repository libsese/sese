#pragma once
#include <sese/db/ResultSet.h>
#include <libpq-fe.h>

namespace sese::db::impl {

    class SESE_DB_API PostgresStmtResultSet : public ResultSet {
    public:
        explicit PostgresStmtResultSet(PGresult *stmt) noexcept;
        ~PostgresStmtResultSet() noexcept override;

        void reset() noexcept override;
        [[nodiscard]]bool next() noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

        [[nodiscard]] int32_t getInteger(size_t index) const noexcept override;
        [[nodiscard]] int64_t getLong(size_t index) const noexcept override;
        [[nodiscard]] std::string_view getString(size_t index) const noexcept override;
        [[nodiscard]] double getDouble(size_t index) const noexcept override;
        [[nodiscard]] float getFloat(size_t index) const noexcept override;

    protected:
        PGresult *stmt;
        int row = 0;
    };

}// namespace sese::db::impl