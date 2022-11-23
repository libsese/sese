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

void impl::SqliteResultSetImpl::reset() noexcept {
    current = 0;
}

bool impl::SqliteResultSetImpl::next() noexcept {
    if (current + 1 > rows) {
        return false;
    } else {
        current += 1;
        return true;
    }
}

const char *impl::SqliteResultSetImpl::getColumnByIndex(size_t index) const noexcept {
    return table[(rows + 1) * current + index];
}

//const char *impl::SqliteResultSetImpl::get(size_t row, size_t column) const noexcept {
//    return table[(rows + 1) * row + column];
//}

size_t impl::SqliteResultSetImpl::getColumns() const noexcept {
    return columns;
}
