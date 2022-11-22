#include <sese/db/impl/SqliteResultSetImpl.h>

using namespace sese::db;

impl::SqliteResultSetImpl::SqliteResultSetImpl(char **table, size_t r, size_t c, char *error) noexcept {
    rows = r;
    columns = c;
    this->table = table;
    this->error = error;
}

impl::SqliteResultSetImpl::~SqliteResultSetImpl() noexcept {
    sqlite3_free_table(table);
    if (this->error) sqlite3_free(error);
}

size_t impl::SqliteResultSetImpl::getRows() const noexcept {
    return rows;
}

size_t impl::SqliteResultSetImpl::getColumns() const noexcept {
    return columns;
}

const char *impl::SqliteResultSetImpl::get(size_t row, size_t column) const noexcept {
    return table[(rows + 1) * row + column];
}
