#pragma once
#include <sese/db/ResultSet.h>
#include <sqlite3.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteResultSetImpl : public ResultSet {
    public:
        SqliteResultSetImpl(char **table, size_t r, size_t c, char *error) noexcept;
        ~SqliteResultSetImpl() noexcept override;

        [[nodiscard]] size_t getRows() const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;
        [[nodiscard]] const char *get(size_t row, size_t column) const noexcept override;

    private:
        size_t rows;
        size_t columns;
        char **table;
        char *error = nullptr;
    };
}// namespace sese::db::impl