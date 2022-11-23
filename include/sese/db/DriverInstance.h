#pragma once
#include <sese/db/Config.h>
#include <sese/db/ResultSet.h>

namespace sese::db {

    class SESE_DB_API DriverInstance {
    public:
        using Ptr = std::unique_ptr<DriverInstance>;

        virtual ~DriverInstance() noexcept = default;
        virtual ResultSet::Ptr executeQuery(const char *sql) const noexcept = 0;
        virtual size_t executeUpdate(const char *sql) const noexcept = 0;
    };
}// namespace sese::db
