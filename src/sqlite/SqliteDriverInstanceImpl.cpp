#include <sqlite/SqliteDriverInstanceImpl.h>
#include <sqlite/SqliteResultSetImpl.h>

using namespace sese::db;

impl::SqliteDriverInstanceImpl::SqliteDriverInstanceImpl(sqlite3 *conn) noexcept {
    this->conn = conn;
}

impl::SqliteDriverInstanceImpl::~SqliteDriverInstanceImpl() noexcept {
    sqlite3_close_v2(conn);
}

ResultSet::Ptr impl::SqliteDriverInstanceImpl::executeQuery(const char *sql) noexcept {
    int rows;
    int columns;
    char **table;
    char *error = nullptr;
    int rt = sqlite3_get_table(conn, sql, &table, &rows, &columns, &error);
    if (0 != rt) return nullptr;
    return std::make_unique<SqliteResultSetImpl>(table, (size_t) rows, (size_t) columns, error);
}

int64_t impl::SqliteDriverInstanceImpl::executeUpdate(const char *sql) noexcept {
    char *error = nullptr;
    auto rt = sqlite3_exec(conn, sql, nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    if (rt == 0) {
        return sqlite3_changes64(conn);
    } else {
        return -1;
    }
}

PreparedStatement::Ptr impl::SqliteDriverInstanceImpl::createStatement(const char *sql) noexcept {
    sqlite3_stmt *stmt;
    if (SQLITE_OK == sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr)) {
        return std::make_unique<impl::SqlitePreparedStatementImpl>(stmt);
    } else {
        return nullptr;
    }
}

int impl::SqliteDriverInstanceImpl::getLastError() const noexcept {
    return sqlite3_errcode(conn);
}

const char *impl::SqliteDriverInstanceImpl::getLastErrorMessage() const noexcept {
    return sqlite3_errmsg(conn);
}

bool impl::SqliteDriverInstanceImpl::setAutoCommit(bool enable) noexcept {
    return true;
}

bool impl::SqliteDriverInstanceImpl::getAutoCommit(bool &status) noexcept {
    status = sqlite3_get_autocommit(conn) != 0;
    return true;
}

bool impl::SqliteDriverInstanceImpl::commit() noexcept {
    char *error = nullptr;
    auto rt = sqlite3_exec(conn, "COMMIT;", nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    return rt == 0;
}

bool impl::SqliteDriverInstanceImpl::rollback() noexcept {
    char *error = nullptr;
    auto rt = sqlite3_exec(conn, "ROLLBACK;", nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    return rt == 0;
}

bool impl::SqliteDriverInstanceImpl::getInsertId(int64_t &id) const noexcept {
    id = (int64_t) sqlite3_last_insert_rowid(conn);
    if (id) {
        return true;
    } else {
        return false;
    }
}

bool impl::SqliteDriverInstanceImpl::begin() noexcept {
    char *error = nullptr;
    auto rt = sqlite3_exec(conn, "BEGIN;", nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    return rt == 0;
}