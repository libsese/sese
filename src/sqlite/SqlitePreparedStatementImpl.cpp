#include <sqlite/SqlitePreparedStatementImpl.h>

using namespace sese::db;

impl::SqlitePreparedStatementImpl::SqlitePreparedStatementImpl(sqlite3_stmt *stmt) noexcept
        : stmt(stmt) {
}

impl::SqlitePreparedStatementImpl::~SqlitePreparedStatementImpl() noexcept {
    sqlite3_finalize(stmt);
}

ResultSet::Ptr impl::SqlitePreparedStatementImpl::executeQuery() noexcept {
    auto rt = sqlite3_step(stmt);
    if (rt == SQLITE_ROW || rt == SQLITE_DONE) {
        sqlite3_reset(stmt);
        this->stmtStatus = true;
        return std::make_unique<SqliteStmtResultSetImpl>(stmt);
    } else {
        return nullptr;
    }
}

int64_t impl::SqlitePreparedStatementImpl::executeUpdate() noexcept {
    if (SQLITE_DONE == sqlite3_step(stmt)) {
        auto conn = sqlite3_db_handle(stmt);
        this->stmtStatus = true;
        return sqlite3_changes64(conn);
    } else {
        return -1;
    }
}

bool impl::SqlitePreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setFloat(uint32_t index, float &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setLong(uint32_t index, int64_t &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int64(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setInteger(uint32_t index, int32_t &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_text(stmt, (int) index, value, -1, nullptr);
}

bool impl::SqlitePreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_null(stmt, (int) index);
}

int impl::SqlitePreparedStatementImpl::getLastError() const noexcept {
    auto conn = sqlite3_db_handle(stmt);
    if (conn) {
        return sqlite3_errcode(conn);
    } else {
        return SQLITE_MISUSE;
    }
}

const char *impl::SqlitePreparedStatementImpl::getLastErrorMessage() const noexcept {
    auto conn = sqlite3_db_handle(stmt);
    if (conn) {
        return sqlite3_errmsg(conn);
    } else {
        return nullptr;
    }
}
