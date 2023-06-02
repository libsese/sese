#include <pgsql/PostgresDriverInstanceImpl.h>
#include <pgsql/PostgresResultSetImpl.h>

using namespace sese::db;

impl::PostgresDriverInstanceImpl::PostgresDriverInstanceImpl(PGconn *conn) noexcept {
    this->conn = conn;
    PQsetNoticeProcessor(conn, nullptr, nullptr);
}

impl::PostgresDriverInstanceImpl::~PostgresDriverInstanceImpl() noexcept {
    PQfinish(conn);
}

ResultSet::Ptr impl::PostgresDriverInstanceImpl::executeQuery(const char *sql) const noexcept {
    PGresult *result = PQexec(conn, sql);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        return nullptr;
    }
    return std::make_unique<PostgresResultSetImpl>(result);
}

int64_t impl::PostgresDriverInstanceImpl::executeUpdate(const char *sql) const noexcept {
    PGresult *result = PQexec(conn, sql);
    return (int64_t) PQcmdTuples(result);
}

PreparedStatement::Ptr impl::PostgresDriverInstanceImpl::createStatement(const char *sql) const noexcept {
    return nullptr;
}