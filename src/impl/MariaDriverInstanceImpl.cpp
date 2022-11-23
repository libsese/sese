#include <sese/db/impl/MariaDriverInstanceImpl.h>

using namespace sese::db;

impl::MariaDriverInstanceImpl::MariaDriverInstanceImpl(MYSQL *conn) noexcept {
    this->conn = conn;
}

impl::MariaDriverInstanceImpl::~MariaDriverInstanceImpl() noexcept {
    mysql_close(conn);
}

ResultSet::Ptr impl::MariaDriverInstanceImpl::executeQuery(const char *sql) const noexcept {
    if (0 != mysql_query(conn, sql)) {
        return nullptr;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    return std::make_unique<MariaResultSetImpl>(result);
}

int64_t impl::MariaDriverInstanceImpl::executeUpdate(const char *sql) const noexcept {
    if (0 != mysql_query(conn, sql)) {
        return -1;
    }
    return (int64_t) mysql_affected_rows(conn);
}

//todo prepared statement
PreparedStatement::Ptr impl::MariaDriverInstanceImpl::createStatement(const char *sql) const noexcept {
    return nullptr;
}
