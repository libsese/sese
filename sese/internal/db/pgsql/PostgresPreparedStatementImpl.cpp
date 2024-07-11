#include <sese/internal/db/pgsql/PostgresPreparedStatementImpl.h>
#include <sstream>
#include <utility>

#ifndef FLOAT8OID
#define FLOAT8OID 701
#define FLOAT4OID 700
#define INT8OID 20
#define INT4OID 23
#define INT2OID 21
#define TEXTOID 25
#define TIMESTAMPOID 1114
#define TIMESTAMPTZOID 1184
#define BOOLOID 16
#define CHAROID 18
#define VARCHAROID 1043
#define DATEOID 1082
#define TIMEOID 1083
#endif

sese::db::impl::PostgresPreparedStatementImpl::PostgresPreparedStatementImpl(
        std::string stmt_name,
        std::string stmt_string,
        uint32_t count,
        PGconn *conn
) noexcept
    : conn(conn), stmtName(std::move(stmt_name)), stmtString(std::move(stmt_string)), count(count), result(nullptr) {
    this->paramTypes = static_cast<Oid *>(malloc(sizeof(Oid) * count));
    memset(this->paramTypes, 0, sizeof(Oid) * count);

    this->paramValues = static_cast<const char **>(malloc(sizeof(const char *) * count));
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

bool sese::db::impl::PostgresPreparedStatementImpl::setDouble(uint32_t index, const double &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = FLOAT8OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setFloat(uint32_t index, const float &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = FLOAT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setInteger(uint32_t index, const int32_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->strings[index - 1] = std::to_string(value);
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = INT4OID;
    return true;
}

bool sese::db::impl::PostgresPreparedStatementImpl::setLong(uint32_t index, const int64_t &value) noexcept {
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
    const std::string DATE_VALUE = text::DateTimeFormatter::format(value, "yyyy-MM-dd HH:mm:ss");
    this->strings[index - 1] = DATE_VALUE;
    this->paramValues[index - 1] = this->strings[index - 1].c_str();
    this->paramTypes[index - 1] = TIMESTAMPOID;
    return true;
}

sese::db::ResultSet::Ptr sese::db::impl::PostgresPreparedStatementImpl::executeQuery() noexcept {
    result = PQexecPrepared(conn, stmtName.c_str(), static_cast<int>(count), paramValues, nullptr, nullptr, 0);
    if (result == nullptr) {
        error = static_cast<int>(PQstatus(conn));
        return nullptr;
    }

    if (const auto STATUS = PQresultStatus(result); STATUS != PGRES_TUPLES_OK) {
        error = static_cast<int>(STATUS);
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
        error = static_cast<int>(PQstatus(conn));
        return -1;
    }

    if (const auto STATUS = PQresultStatus(result); STATUS != PGRES_COMMAND_OK) {
        error = static_cast<int>(STATUS);
        return -1;
    }

    const auto RT = reinterpret_cast<int64_t>(PQcmdTuples(result));
    PQclear(result);
    result = nullptr;
    error = 0;
    return RT;
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
    const auto META = PQdescribePrepared(conn, stmtName.c_str());
    if (META == nullptr || PQnfields(META) <= static_cast<int>(index)) {
        return false;
    }
    switch (PQftype(META, static_cast<int>(index))) {
        case BOOLOID:
            type = MetadataType::BOOLEAN;
            break;
        case TEXTOID:
        case CHAROID:
        case VARCHAROID:
            type = MetadataType::TEXT;
            break;
        case INT8OID:
            type = MetadataType::LONG;
            break;
        case INT4OID:
            type = MetadataType::INTEGER;
            break;
        case INT2OID:
            type = MetadataType::SHORT;
            break;
        case FLOAT8OID:
            type = MetadataType::FLOAT;
            break;
        case FLOAT4OID:
            type = MetadataType::DOUBLE;
            break;
        case DATEOID:
            type = MetadataType::DATE;
            break;
        case TIMEOID:
            type = MetadataType::TIME;
            break;
        case TIMESTAMPTZOID:
        case TIMESTAMPOID:
            type = MetadataType::DATE_TIME;
            break;
        default:
            type = MetadataType::UNKNOWN;
            break;
    }
    return true;
}

int64_t sese::db::impl::PostgresPreparedStatementImpl::getColumnSize(uint32_t index) noexcept {
    const auto META = PQdescribePrepared(conn, stmtName.c_str());
    if (META == nullptr || PQnfields(META) <= static_cast<int>(index)) {
        return -1;
    }
    return PQfsize(META, static_cast<int>(index));
}
