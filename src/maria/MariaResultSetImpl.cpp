#include <maria/MariaResultSetImpl.h>

using namespace sese::db;

impl::MariaResultSetImpl::MariaResultSetImpl(MYSQL_RES *res) noexcept : ResultSet() {
    this->res = res;
}

impl::MariaResultSetImpl::~MariaResultSetImpl() noexcept {
    mysql_free_result(res);
}

void impl::MariaResultSetImpl::reset() noexcept {
    mysql_data_seek(res, 0);
}

bool impl::MariaResultSetImpl::next() noexcept {
    row = mysql_fetch_row(res);
    return row != nullptr;
}

//const char *impl::MariaResultSetImpl::getColumnByIndex(size_t index) const noexcept {
//    return row[index];
//}

size_t impl::MariaResultSetImpl::getColumns() const noexcept {
    return mysql_num_fields(res);
}

int32_t impl::MariaResultSetImpl::getInteger(size_t index) const noexcept {
    char *end;
    return (int32_t) std::strtol(row[index], &end, 10);
}

int64_t impl::MariaResultSetImpl::getLong(size_t index) const noexcept {
    char *end;
    return std::strtoll(row[index], &end, 10);
}

std::string_view impl::MariaResultSetImpl::getString(size_t index) const noexcept {
    return {row[index]};
}

double impl::MariaResultSetImpl::getDouble(size_t index) const noexcept {
    char *end;
    return std::strtod(row[index], &end);
}
float impl::MariaResultSetImpl::getFloat(size_t index) const noexcept {
    char *end;
    return std::strtof(row[index], &end);
}
