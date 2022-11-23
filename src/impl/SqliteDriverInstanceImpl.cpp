#include <sese/db/impl/SqliteDriverInstanceImpl.h>

using namespace sese::db;

impl::SqliteDriverInstance::SqliteDriverInstance(sqlite3 *conn) noexcept {
    this->conn = conn;
}

impl::SqliteDriverInstance::~SqliteDriverInstance() noexcept {
    sqlite3_close_v2(conn);
}

ResultSet::Ptr impl::SqliteDriverInstance::executeQuery(const char *sql) const noexcept {
    int rows;
    int columns;
    char **table;
    char *error = nullptr;
    int rt = sqlite3_get_table(conn, sql, &table, &rows, &columns, &error);
    if (0 != rt) return nullptr;
    return std::make_unique<SqliteResultSetImpl>(table, (size_t) rows, (size_t) columns, error);
}

size_t impl::SqliteDriverInstance::executeUpdate(const char *sql) const noexcept {
    char *error = nullptr;
    auto rt = sqlite3_exec(conn, sql, nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    if (rt == 0) {
        return sqlite3_changes64(conn);
    } else {
        return -1;
    }
}
