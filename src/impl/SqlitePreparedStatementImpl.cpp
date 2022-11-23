#include <sese/db/impl/SqlitePreparedStatementImpl.h>

using namespace sese::db;

impl::SqlitePreparedStatementImpl::SqlitePreparedStatementImpl(sqlite3_stmt *stmt) noexcept : stmt(stmt) {
}

impl::SqlitePreparedStatementImpl::~SqlitePreparedStatementImpl() noexcept {
    sqlite3_finalize(stmt);
}

ResultSet::Ptr impl::SqlitePreparedStatementImpl::executeQuery() noexcept {
    auto rt = sqlite3_step(stmt);
    if (rt == SQLITE_ROW || rt == SQLITE_DONE) {
        sqlite3_reset(stmt);
        return std::make_unique<SqliteStmtResultSetImpl>(stmt);
    } else {
        return nullptr;
    }
}

int64_t impl::SqlitePreparedStatementImpl::executeUpdate() noexcept {
    if (SQLITE_DONE == sqlite3_step(stmt)) {
        auto conn = sqlite3_db_handle(stmt);
        return sqlite3_changes64(conn);
    } else {
        return -1;
    }
}

int64_t impl::SqlitePreparedStatementImpl::setDouble(uint32_t index, double value) noexcept {
    return sqlite3_bind_double(stmt, (int) index, value);
}

int64_t impl::SqlitePreparedStatementImpl::setInteger(uint32_t index, int64_t value) noexcept {
    return sqlite3_bind_int64(stmt, (int) index, value);
}

int64_t impl::SqlitePreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    return sqlite3_bind_text(stmt, (int) index, value, -1, nullptr);
}
int64_t impl::SqlitePreparedStatementImpl::setNull(uint32_t index) noexcept {
    return sqlite3_bind_null(stmt, (int) index);
}
