#pragma once
#include <sese/db/Config.h>

namespace sese::db {

    class SESE_DB_API ResultSet {
    public:
        using Ptr = std::unique_ptr<ResultSet>;

        virtual ~ResultSet() noexcept = default;

        [[nodiscard]] virtual size_t getRows() const noexcept = 0;
        [[nodiscard]] virtual size_t getColumns() const noexcept = 0;

        [[nodiscard]] virtual const char *get(size_t row, size_t column) const noexcept = 0;
    };
}