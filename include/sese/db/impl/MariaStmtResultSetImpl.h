#pragma once
#include <sese/db/ResultSet.h>
#include <mysql.h>

namespace sese::db::impl {

    class SESE_DB_API MariaStmtResultSet : public ResultSet {
    public:
        explicit MariaStmtResultSet(MYSQL_STMT *stmt) noexcept;

        void reset() noexcept override;
        [[nodiscard]]bool next() noexcept override;
        [[nodiscard]] const char *getColumnByIndex(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

    protected:
        MYSQL_STMT *stmt;
        size_t columns = 0;
        MYSQL_BIND *param = nullptr;
    };

}// namespace sese::db::impl