#include <pgsql/PostgresStmtResultSetImpl.h>

using namespace sese::db;

impl::PostgresStmtResultSet::PostgresStmtResultSet(PGresult *stmt) noexcept {
    this->stmt = stmt;

}

impl::PostgresStmtResultSet::~PostgresStmtResultSet() noexcept {
    PQclear(stmt);
}

void impl::PostgresStmtResultSet::reset() noexcept {

}

bool impl::PostgresStmtResultSet::next() noexcept {
    if (1 == PQgetisnull(stmt, row, 0)) {
        return false;
    }
    row = row + 1;
    return true;
}

size_t impl::PostgresStmtResultSet::getColumns() const noexcept {
    return PQnfields(stmt);
}

int32_t impl::PostgresStmtResultSet::getInteger(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(stmt, row - 1, (int) index), &end, 10);
}

int64_t impl::PostgresStmtResultSet::getLong(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(stmt, row - 1, (int) index), &end, 10);
}

std::string_view impl::PostgresStmtResultSet::getString(size_t index) const noexcept {
    return PQgetvalue(stmt, row - 1, (int) index);
}

double impl::PostgresStmtResultSet::getDouble(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(stmt, row - 1, (int) index), &end, 10);
}

float impl::PostgresStmtResultSet::getFloat(size_t index) const noexcept {
    char *end;
    return std::strtof(PQgetvalue(stmt, row - 1, (int) index), &end);
}
