#pragma once
#include <sese/db/ResultSet.h>
#include <mysql.h>

namespace sese::db::impl {

    class SESE_DB_API MariaResultSetImpl : public ResultSet {
    public:
        explicit MariaResultSetImpl(MYSQL_RES *res) noexcept;
        ~MariaResultSetImpl() noexcept override;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] const char *getColumnByIndex(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

    private:
        MYSQL_RES *res;
        MYSQL_ROW row = nullptr;
    };
}// namespace sese::db::impl