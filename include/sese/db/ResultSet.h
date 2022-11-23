#pragma once
#include <sese/db/Config.h>

namespace sese::db {

    class SESE_DB_API ResultSet {
    public:
        using Ptr = std::unique_ptr<ResultSet>;

        virtual ~ResultSet() noexcept = default;
        virtual void reset() noexcept = 0;
        [[nodiscard]] virtual bool next() noexcept = 0;
        [[nodiscard]] virtual const char *getColumnByIndex(size_t index) const noexcept = 0;
        [[nodiscard]] virtual size_t getColumns() const noexcept = 0;
    };
}