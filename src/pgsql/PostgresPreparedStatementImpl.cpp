#include <pgsql/PostgresPreparedStatementImpl.h>
#include <catalog/pg_type_d.h>
#include <sstream>

sese::db::impl::PostgresPreparedStatementImpl::PostgresPreparedStatementImpl(
        const std::string &stmtName,
        const std::string &stmtString,
        uint32_t count,
        PGconn *conn
) noexcept
    : stmtName(stmtName), stmtString(stmtString), count(count), conn(conn), result(nullptr) {
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

bool sese::db::impl::PostgresPreparedStatementImpl::setDateTime(uint32_t index, const sese::DateTime &value) noexcept {
    if (index - 1 >= count) return false;
    std::string timeValue = text::DateTimeFormatter::format(value, "yyyy-MM-dd HH:mm:ss");
    this->paramValues[index - 1] = timeValue.c_str();
    this->paramTypes[index - 1] = TIMESTAMPOID;
    return true;
}

sese::db::ResultSet::Ptr sese::db::impl::PostgresPreparedStatementImpl::executeQuery() noexcept {
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

bool sese::db::impl::PostgresPreparedStatementImpl::getColumnType(uint32_t index, sese::db::MetadataType &type) noexcept {
    auto meta = PQdescribePrepared(conn, stmtName.c_str());
    if (meta == nullptr || PQnfields(meta) <= index) {
        return -1;
    }
    auto oid = PQftype(meta, static_cast<int>(index));
    switch (oid) {
        case BOOLOID:
            type = MetadataType::Boolean;
            break;
        case TEXTOID:
        case CHAROID:
        case VARCHAROID:
            type = MetadataType::Text;
            break;
        case INT8OID:
            type = MetadataType::Long;
            break;
        case INT4OID:
            type = MetadataType::Integer;
            break;
        case INT2OID:
            type = MetadataType::Short;
            break;
        case FLOAT8OID:
            type = MetadataType::Float;
            break;
        case FLOAT4OID:
            type = MetadataType::Double;
            break;
        case DATEOID:
            type = MetadataType::Date;
            break;
        case TIMEOID:
            type = MetadataType::Time;
            break;
        case TIMESTAMPTZOID:
        case TIMESTAMPOID:
            type = MetadataType::DateTime;
            break;
        default:
            type = MetadataType::Unknown;
            break;
    }
    return true;
}

int64_t sese::db::impl::PostgresPreparedStatementImpl::getColumnSize(uint32_t index) noexcept {
    auto meta = PQdescribePrepared(conn, stmtName.c_str());
    if (meta == nullptr || PQnfields(meta) <= index) {
        return -1;
    }
    return PQfsize(meta, index);
}
