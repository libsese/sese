#pragma once
#include <sese/db/ResultSet.h>
#include <sqlite3.h>

namespace sese::db::impl {

    class SESE_DB_API SqliteStmtResultSetImpl : public ResultSet {
    public:
        explicit SqliteStmtResultSetImpl(sqlite3_stmt *stmt) noexcept;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] const char *getColumnByIndex(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

    protected:
        sqlite3_stmt *stmt;
    };

}// namespace sese::db::impl