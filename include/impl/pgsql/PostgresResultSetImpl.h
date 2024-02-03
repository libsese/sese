#pragma once
#include <sese/db/ResultSet.h>
#include <libpq-fe.h>

namespace sese::db::impl {
    class SESE_DB_API PostgresResultSetImpl : public ResultSet {
    public:
        explicit PostgresResultSetImpl(PGresult *res) noexcept;
        ~PostgresResultSetImpl() noexcept override;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

        [[nodiscard]] int32_t getInteger(size_t index) const noexcept override;
        [[nodiscard]] int64_t getLong(size_t index) const noexcept override;
        [[nodiscard]] std::string_view getString(size_t index) const noexcept override;
        [[nodiscard]] double getDouble(size_t index) const noexcept override;
        [[nodiscard]] float getFloat(size_t index) const noexcept override;
        [[nodiscard]] std::optional<sese::DateTime> getDateTime(size_t index) const noexcept override;
    protected:
        PGresult *res;
        int row;
        int totalRow;
    };
}// namespace sese::db::impl