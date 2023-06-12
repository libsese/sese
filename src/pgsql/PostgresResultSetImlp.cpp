#include <pgsql/PostgresResultSetImpl.h>

using namespace sese::db;

impl::PostgresResultSetImpl::PostgresResultSetImpl(PGresult *res) noexcept {
    this->res = res;
}

impl::PostgresResultSetImpl::~PostgresResultSetImpl() noexcept {
    PQclear(res);
}

void impl::PostgresResultSetImpl::reset() noexcept {
    row = 0;
}

bool impl::PostgresResultSetImpl::next() noexcept {
    if (1 == PQgetisnull(res, row, 0)) {
        return false;
    }
    row = row + 1;
    return true;
}

size_t impl::PostgresResultSetImpl::getColumns() const noexcept {
    return PQnfields(res);
}

int32_t impl::PostgresResultSetImpl::getInteger(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(res, row - 1, (int) index), &end, 10);
}

int64_t impl::PostgresResultSetImpl::getLong(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(res, row - 1, (int) index), &end, 10);
}

std::string_view impl::PostgresResultSetImpl::getString(size_t index) const noexcept {
    return PQgetvalue(res, row - 1, (int) index);
}

double impl::PostgresResultSetImpl::getDouble(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(res, row - 1, (int) index), &end, 10);
}

float impl::PostgresResultSetImpl::getFloat(size_t index) const noexcept {
    char *end;
    return std::strtof(PQgetvalue(res, row - 1, (int) index), &end);
}
