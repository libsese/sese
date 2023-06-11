#include <pgsql/PostgresPreparedStatementImpl.h>
#include <catalog/pg_type_d.h>
#include <sstream>
#include <random>

sese::db::impl::PostgresPreparedStatementImpl::PostgresPreparedStatementImpl(const std::string &stmt, int count, PGconn *conn) noexcept {
    this->stmt = stmt;
    this->count = count;
    this->conn = conn;
    this->paramTypes = (Oid *) malloc(sizeof(Oid) * count);
    memset(this->paramTypes, 0, sizeof(Oid) * count);
    this->paramValues = (const char **) malloc(sizeof(const char *) * count);
    this->strings = new std::string[count];
}

sese::db::impl::PostgresPreparedStatementImpl::~PostgresPreparedStatementImpl() noexcept {
    free(paramTypes);
    free(paramValues);
    delete[] this->strings;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = FLOAT8OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setFloat(uint32_t index, float &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = FLOAT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setInteger(uint32_t index, int32_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = INT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setLong(uint32_t index, int64_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = INT8OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (index - 1 >= count) return false;
    this->paramValues[index - 1] = value;
    this->paramTypes[index - 1] = TEXTOID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (index - 1 >= count) return false;
    this->paramTypes[index - 1] = NULL;
    return true;
}

sese::db::ResultSet::Ptr sese::db::impl::PostgresPreparedStatementImpl::executeQuery() noexcept {
    const Oid *containerType = paramTypes;
    std::stringstream stringStream;
    std::random_device rand;
    stringStream << "sese_stmt_" << rand();


    PGresult *resPrepare = PQprepare(conn, stringStream.str().c_str(), stmt.c_str(), count, containerType);
    if (PQresultStatus(resPrepare) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(conn));
        return nullptr;
    }

    PGresult *resExec = PQexecPrepared(conn, stringStream.str().c_str(), count, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(resExec) != PGRES_TUPLES_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(conn));
        return nullptr;
    }
    return std::make_unique<impl::PostgresStmtResultSet>(resExec);
}

int64_t sese::db::impl::PostgresPreparedStatementImpl::executeUpdate() noexcept {
    const Oid *containerType = paramTypes;
    std::stringstream stringStream;
    std::random_device rand;
    stringStream << "sese_stmt_" << rand();

    PGresult *resPrepare = PQprepare(conn, stringStream.str().c_str(), stmt.c_str(), count, containerType);
    if (PQresultStatus(resPrepare) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(conn));
        return -1;
    }

    PGresult *resExec = PQexecPrepared(conn, stringStream.str().c_str(), count, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(resExec) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(conn));
        return -1;
    }
    return (int64_t) PQcmdTuples(resExec);
}