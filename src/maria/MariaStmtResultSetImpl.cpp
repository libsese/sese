#include <maria/MariaStmtResultSetImpl.h>
#include <maria/MariaPreparedStatementImpl.h>

sese::db::impl::MariaStmtResultSet::MariaStmtResultSet(MYSQL_STMT *stmt, MYSQL_BIND *row, size_t count) noexcept {
    this->stmt = stmt;
    this->row = row;
    this->columns = count;
}

sese::db::impl::MariaStmtResultSet::~MariaStmtResultSet() noexcept {
    MariaPreparedStatementImpl::freeBindStruct(this->row, this->columns);
}

void sese::db::impl::MariaStmtResultSet::reset() noexcept {
    mysql_stmt_data_seek(stmt, 0);
}

bool sese::db::impl::MariaStmtResultSet::next() noexcept {
    return 0 == mysql_stmt_fetch(stmt);
}

size_t sese::db::impl::MariaStmtResultSet::getColumns() const noexcept {
    return columns;
}

int64_t sese::db::impl::MariaStmtResultSet::getLong(size_t index) const noexcept {
    int64_t value;
    value = *(int64_t *)row[index].buffer;
    return value;
}

int32_t sese::db::impl::MariaStmtResultSet::getInteger(size_t index) const noexcept {
    int32_t value;
    value = *(int32_t *)row[index].buffer;
    return value;
}

std::string_view sese::db::impl::MariaStmtResultSet::getString(size_t index) const noexcept {
    return {(const char *) row[index].buffer};
}

double sese::db::impl::MariaStmtResultSet::getDouble(size_t index) const noexcept {
    double value;
    value = *(double *)row[index].buffer;
    return value;
}

float sese::db::impl::MariaStmtResultSet::getFloat(size_t index) const noexcept {
    float value;
    value = *(float *)row[index].buffer;
    return value;
}
