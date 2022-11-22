#include <sese/db/DriverManager.h>

#ifdef HAS_SQLITE
#include <sese/db/impl/SqliteDriverInstanceImpl.h>
#endif

using namespace sese::db;

DriverInstance::Ptr DriverManager::getInstance(sese::db::DatabaseType type, const char *connectionString) noexcept {
    switch (type) {
#ifdef HAS_SQLITE
        case DatabaseType::Sqlite: {
            sqlite3 *conn;
            int rt = sqlite3_open_v2(connectionString, &conn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            if (rt != 0) return nullptr;
            return std::make_unique<impl::SqliteDriverInstance>(conn);
        }
#endif
        default:
            return nullptr;
    }
}