#include <sese/db/impl/MariaStmtResultSetImpl.h>

sese::db::impl::MariaStmtResultSet::MariaStmtResultSet(MYSQL_STMT *stmt) noexcept {
    this->stmt = stmt;
}

void sese::db::impl::MariaStmtResultSet::reset() noexcept {

}

bool sese::db::impl::MariaStmtResultSet::next() noexcept {
    return false;
}

const char *sese::db::impl::MariaStmtResultSet::getColumnByIndex(size_t index) const noexcept {
    return nullptr;
}

size_t sese::db::impl::MariaStmtResultSet::getColumns() const noexcept {
    return 0;
}
