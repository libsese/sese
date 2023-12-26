#include <pgsql/PostgresPreparedStatementImpl.h>
#include <catalog/pg_type_d.h>
#include <sstream>

sese::db::impl::PostgresPreparedStatementImpl::PostgresPreparedStatementImpl(const std::string &stmt, uint32_t count, PGconn *conn) noexcept {
    this->stmt = stmt;
    this->count = count;
    this->conn = conn;
    this->result = nullptr;
    this->stmtStatus = false;
    this->paramTypes = (Oid *) malloc(sizeof(Oid) * count);
    memset(this->paramTypes, 0, sizeof(Oid) * count);
    this->paramValues = (const char **) malloc(sizeof(const char *) * count);
    this->strings = new std::string[count];
}

sese::db::impl::PostgresPreparedStatementImpl::~PostgresPreparedStatementImpl() noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
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
    this->paramValues[index - 1] = nullptr;
    this->paramTypes[index - 1] = 0;
    return true;
}

/// 此函数用于创建驱动实现的具体预处理语句。
/// 创建成功将会返回 true 并且 stmtStatus 为 true；
/// 创建失败将会返回 false 并且 stmtStatus 为 false。
bool sese::db::impl::PostgresPreparedStatementImpl::createStmt() noexcept {
    std::random_device rd;
    std::mt19937 e{rd()};
    std::uniform_int_distribution<int> discreteDistribution(1,65535);

    this->stmtName = "sese_stmt_" + std::to_string(discreteDistribution(e));
    const Oid *containerType = paramTypes;

    if (result) {
        PQclear(result);
        result = nullptr;
    }

    result = PQprepare(conn, stmtName.c_str(), stmt.c_str(), static_cast<int>(count), containerType);
    if (result == nullptr) {
        error = (int) PQstatus(conn);
        this->stmtStatus = false;
        return false;
    }

    auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        error = (int) status;
        this->stmtStatus = false;
        return false;
    }

    this->stmtStatus = true;
    PQclear(result);
    result = nullptr;

    return true;
}

/// 执行前将判断驱动预处理语句是否已经被创建，
/// 如果 预处理语句不存在且尝试创建预处理语句失败 则 返回并返回空指针，
/// 反之继续执行接下来的逻辑
sese::db::ResultSet::Ptr sese::db::impl::PostgresPreparedStatementImpl::executeQuery() noexcept {
    if (!this->stmtStatus && !createStmt()) {
        return nullptr;
    }

    result = PQexecPrepared(conn, stmtName.c_str(), static_cast<int>(count), paramValues, nullptr, nullptr, 0);
    if (result == nullptr) {
        error = (int) PQstatus(conn);
        return nullptr;
    }

    auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        error = (int) status;
        return nullptr;
    }


    auto rt = std::make_unique<impl::PostgresResultSetImpl>(result);
    error = 0;
    result = nullptr;
    return rt;
}

int64_t sese::db::impl::PostgresPreparedStatementImpl::executeUpdate() noexcept {
    if (!this->stmtStatus && !createStmt()) {
        return -1;
    }

    result = PQexecPrepared(conn, stmtName.c_str(), static_cast<int>(count), paramValues, nullptr, nullptr, 0);
    if (result == nullptr) {
        error = (int) PQstatus(conn);
        return -1;
    }

    auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        error = (int) status;
        return -1;
    }


    auto rt = (int64_t) PQcmdTuples(result);
    PQclear(result);
    result = nullptr;
    error = 0;
    return rt;
}

int sese::db::impl::PostgresPreparedStatementImpl::getLastError() const noexcept {
    return error;
}

const char *sese::db::impl::PostgresPreparedStatementImpl::getLastErrorMessage() const noexcept {
    if (result) {
        return PQresultErrorMessage(result);
    } else {
        return PQerrorMessage(conn);
    }
}
