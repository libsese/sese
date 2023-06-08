#include <pgsql/PostgresDriverInstanceImpl.h>
#include <pgsql/PostgresResultSetImpl.h>
#include <sstream>

using namespace sese::db;

impl::PostgresDriverInstanceImpl::PostgresDriverInstanceImpl(PGconn *conn) noexcept {
    this->conn = conn;
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
    int count = 0;
    std::stringstream stringBuilder;
    for (size_t i = 0; i < strlen(sql); ++i) {
        if (sql[i] == '?') {
            count++;
            stringBuilder << '$' << count;
        } else {
            stringBuilder << sql[i];
        }
    }
    std::string str = stringBuilder.str();
    str.erase(std::remove(str.begin(), str.end(), '\0'), str.end());

    return std::make_unique<impl::PostgresPreparedStatementImpl>(str, count, conn);
}