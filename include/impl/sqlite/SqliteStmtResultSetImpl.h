#pragma once
#include <sese/db/ResultSet.h>
#include <sqlite3.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteStmtResultSetImpl final : public ResultSet {
    public:
        explicit SqliteStmtResultSetImpl(sqlite3_stmt *stmt) noexcept;

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
        sqlite3_stmt *stmt;
    };

}// namespace sese::db::impl