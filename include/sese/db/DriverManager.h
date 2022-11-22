#pragma once
#include <sese/db/DriverInstance.h>

namespace sese::db {
    enum class DatabaseType {
        MySql,
        Maria,
        Sqlite
    };

    class SESE_DB_API DriverManager {
    public:
        static DriverInstance::Ptr getInstance(DatabaseType type, const char *connString) noexcept;
    };
}// namespace sese::db