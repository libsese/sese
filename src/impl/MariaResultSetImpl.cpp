#include <sese/db/impl/MariaResultSetImpl.h>

using namespace sese::db;

impl::MariaResultSetImpl::MariaResultSetImpl(MYSQL_RES *res) noexcept {
    this->res = res;
}

impl::MariaResultSetImpl::~MariaResultSetImpl() noexcept {
    mysql_free_result(res);
}

void impl::MariaResultSetImpl::reset() noexcept {
    mysql_field_seek(res, 0);
}

bool impl::MariaResultSetImpl::next() noexcept {
    row = mysql_fetch_row(res);
    return row != nullptr;
}

const char *impl::MariaResultSetImpl::getColumnByIndex(size_t index) const noexcept {
    return row[index];
}

size_t impl::MariaResultSetImpl::getColumns() const noexcept {
    return mysql_num_fields(res);
}