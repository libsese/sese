#include <pgsql/PostgresPreparedStatementImpl.h>
#include <catalog/pg_type_d.h>
#include <sstream>

sese::db::impl::PostgresPreparedStatementImpl::PostgresPreparedStatementImpl(const std::string &stmt, int count, PGconn *conn) noexcept {
    this->stmt = stmt;
    this->count = count;
    this->conn = conn;
    this->paramTypes = (Oid *) malloc(sizeof(Oid) * count );
    memset(this->paramTypes, 0, sizeof(Oid) * count);
    this->paramValues = (char *) malloc(sizeof(char) * count);
    memset(this->paramValues, 0, sizeof(char) * count);
}

sese::db::impl::PostgresPreparedStatementImpl::~PostgresPreparedStatementImpl() noexcept {
    free(paramTypes);
    free(paramValues);
}

bool sese::db::impl::PostgresPreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = (char) value;
    this->paramTypes[index - 1] = FLOAT8OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setFloat(uint32_t index, float &value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = (char) value;
    this->paramTypes[index - 1] = FLOAT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setInteger(uint32_t index, int32_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = (char) value;
    this->paramTypes[index - 1] = INT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setLong(uint32_t index, int64_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = (char) value;
    this->paramTypes[index - 1] = INT8OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = *value;
    this->paramTypes[index - 1] = TEXTOID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (index - 1 >= count) return false;
    this->paramTypes[index - 1] = NULL;
    return true;
}

sese::db::ResultSet::Ptr sese::db::impl::PostgresPreparedStatementImpl::executeQuery() noexcept {
    const char * const *container = &paramValues;
    std::stringstream stringStream;
    srand((unsigned)time(nullptr));
    stringStream << "sese" << "_stmt_" << rand();
//  创建预处理语句失败
    PQprepare(conn, stringStream.str().c_str(), stmt.c_str(), count, paramTypes);

    PGresult *res = PQexecPrepared(conn, stringStream.str().c_str(), count, container, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
        return nullptr;
    }
    return std::make_unique<impl::PostgresStmtResultSet>(res);
}

int64_t sese::db::impl::PostgresPreparedStatementImpl::executeUpdate() noexcept {
    const char * const *container = &paramValues;
    std::stringstream stringStream;
    srand((unsigned)time(nullptr));
    stringStream << "sese" << "_stmt_" << rand();

    PQprepare(conn, stringStream.str().c_str(), stmt.c_str(), count, paramTypes);
    PGresult *res = PQexecPrepared(conn, stringStream.str().c_str(), count, container, nullptr, nullptr, 0);
    return (int64_t) PQcmdTuples(res);
}