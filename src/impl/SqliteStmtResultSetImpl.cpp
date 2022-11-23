#include <sese/db/impl/SqliteStmtResultSetImpl.h>

using namespace sese::db;

void impl::SqliteStmtResultSetImpl::reset() noexcept {
    sqlite3_reset(stmt);
}

bool impl::SqliteStmtResultSetImpl::next() noexcept {
    return sqlite3_step(stmt) == SQLITE_ROW;
}

const char *impl::SqliteStmtResultSetImpl::getColumnByIndex(size_t index) const noexcept {
    return (const char *) sqlite3_column_text(stmt, (int) index);
}

size_t impl::SqliteStmtResultSetImpl::getColumns() const noexcept {
    return sqlite3_column_count(stmt);
}

impl::SqliteStmtResultSetImpl::SqliteStmtResultSetImpl(sqlite3_stmt *stmt) noexcept {
    this->stmt = stmt;
}