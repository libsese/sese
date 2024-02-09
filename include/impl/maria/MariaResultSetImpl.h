#pragma once

#include <sese/db/ResultSet.h>

#include <mysql.h>

namespace sese::db::impl {

    class SESE_DB_API MariaResultSetImpl final : public ResultSet {
    public:
        explicit MariaResultSetImpl(MYSQL_RES *res) noexcept;
        ~MariaResultSetImpl() noexcept override;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] bool isNull(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

        [[nodiscard]] int32_t getInteger(size_t index) const noexcept override;
        [[nodiscard]] int64_t getLong(size_t index) const noexcept override;
        [[nodiscard]] std::string_view getString(size_t index) const noexcept override;
        [[nodiscard]] double getDouble(size_t index) const noexcept override;
        [[nodiscard]] float getFloat(size_t index) const noexcept override;
        [[nodiscard]] std::optional<sese::DateTime> getDateTime(size_t index) const noexcept override;

    protected:
        MYSQL_RES *res;
        MYSQL_ROW row = nullptr;
    };

}// namespace sese::db::impl