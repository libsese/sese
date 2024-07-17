#include <sese/internal/db/pgsql/PostgresResultSetImpl.h>
#include <sese/text/DateTimeParser.h>

using namespace sese::db;

impl::PostgresResultSetImpl::PostgresResultSetImpl(PGresult *res) noexcept {
    this->res = res;
    this->row = 0;
    this->totalRow = PQntuples(res);
}

impl::PostgresResultSetImpl::~PostgresResultSetImpl() noexcept {
    PQclear(res);
}

void impl::PostgresResultSetImpl::reset() noexcept {
    row = 0;
}

bool impl::PostgresResultSetImpl::next() noexcept {
    if (totalRow <= 0) return false;

    if (row + 1 <= totalRow) {
        row += 1;
        return true;
    } else {
        return false;
    }
}

size_t impl::PostgresResultSetImpl::getColumns() const noexcept {
    return PQnfields(res);
}

int32_t impl::PostgresResultSetImpl::getInteger(size_t index) const noexcept {
    char *end;
    return static_cast<int32_t>(std::strtol(PQgetvalue(res, row - 1, static_cast<int>(index)), &end, 10));
}

int64_t impl::PostgresResultSetImpl::getLong(size_t index) const noexcept {
    char *end;
    return std::strtol(PQgetvalue(res, row - 1, static_cast<int>(index)), &end, 10);
}

std::string_view impl::PostgresResultSetImpl::getString(size_t index) const noexcept {
    return PQgetvalue(res, row - 1, static_cast<int>(index));
}

double impl::PostgresResultSetImpl::getDouble(size_t index) const noexcept {
    char *end;
    return std::strtod(PQgetvalue(res, row - 1, static_cast<int>(index)), &end);
}

float impl::PostgresResultSetImpl::getFloat(size_t index) const noexcept {
    char *end;
    return std::strtof(PQgetvalue(res, row - 1, static_cast<int>(index)), &end);
}
std::optional<sese::DateTime> impl::PostgresResultSetImpl::getDateTime(size_t index) const noexcept {
    std::optional<sese::DateTime> rt = text::DateTimeParser::parse("yyyy-MM-dd HH:mm:ss", PQgetvalue(res, row - 1, static_cast<int>(index)));
    return rt;
}
bool impl::PostgresResultSetImpl::isNull(size_t index) const noexcept {
    if (PQgetisnull(res, row - 1, static_cast<int>(index))) return true;
    return false;
}
