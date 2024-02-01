#include <sqlite/SqlitePreparedStatementImpl.h>

#include <sese/record/Marco.h>

#undef SESE_DEBUG
#define SESE_DEBUG(str, ...)

using namespace sese::db;

const char *impl::SqlitePreparedStatementImpl::IntegerAffinitySet[9]{
        "INT",
        "INTEGER",
        "TINYINT",
        "SMALLINT",
        "MEDIUMINT",
        "BIGINT",
        "UNSIGNED BIG INT",
        "INT2",
        "INT8"
};

const char *impl::SqlitePreparedStatementImpl::TextAffinitySet[8]{
        "CHARACTER",
        "VARCHAR",
        "VARYING CHARACTER",
        "NCHAR",
        "NATIVE CHARACTER",
        "NVARCHAR",
        "TEXT",
        "CLOB"
};

const char *impl::SqlitePreparedStatementImpl::RealAffinitySet[6]{
        "REAL",
        "DOUBLE",
        "DOUBLE PRECISION",
        "FLOAT",
        "NUMERIC",
        "DECIMAL"
};

inline bool in(const char **set, int size, const char *target) {
    for (int i = 0; i < size; ++i) {
        if (sese::strcmpDoNotCase(set[i], target)) {
            return true;
        }
    }
    return false;
}

std::string impl::SqlitePreparedStatementImpl::splitBefore(const std::string &str) {
    auto pos = str.find_first_of('(');
    if (std::string::npos == pos) {
        return str;
    }
    return str.substr(0, pos);
}

impl::SqlitePreparedStatementImpl::SqlitePreparedStatementImpl(sqlite3_stmt *stmt, size_t count) noexcept
    : stmt(stmt) {
    this->count = count;
    this->isDatetime = (bool *) malloc(count);
    memset(this->isDatetime, false, count);
    this->buffer = (void **) malloc(sizeof(void *) * count);
}

impl::SqlitePreparedStatementImpl::~SqlitePreparedStatementImpl() noexcept {
    for (int i = 0; i < count; ++i) {
        if (this->isDatetime[i]) {
            SESE_DEBUG("free buffer addr: %p", this->buffer[i]);
            free(buffer[i]);
        }
    }
    free(isDatetime);
    sqlite3_finalize(stmt);
}

ResultSet::Ptr impl::SqlitePreparedStatementImpl::executeQuery() noexcept {
    auto rt = sqlite3_step(stmt);
    if (rt == SQLITE_ROW || rt == SQLITE_DONE) {
        sqlite3_reset(stmt);
        this->stmtStatus = true;
        return std::make_unique<SqliteStmtResultSetImpl>(stmt);
    } else {
        return nullptr;
    }
}

int64_t impl::SqlitePreparedStatementImpl::executeUpdate() noexcept {
    if (SQLITE_DONE == sqlite3_step(stmt)) {
        auto conn = sqlite3_db_handle(stmt);
        this->stmtStatus = true;
        return sqlite3_changes64(conn);
    } else {
        return -1;
    }
}

bool impl::SqlitePreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setFloat(uint32_t index, float &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setLong(uint32_t index, int64_t &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int64(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setInteger(uint32_t index, int32_t &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        SESE_DEBUG("free buffer addr: %p", this->buffer[index - 1]);
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int(stmt, (int) index, value);
}

bool impl::SqlitePreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_text(stmt, (int) index, value, -1, nullptr);
}

bool impl::SqlitePreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isDatetime[index - 1]) {
        free(this->buffer[index - 1]);
        this->isDatetime[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_null(stmt, (int) index);
}

bool impl::SqlitePreparedStatementImpl::setDateTime(uint32_t index, const sese::DateTime &value) noexcept {
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;

    if (!this->isDatetime[index - 1]) {
        this->buffer[index - 1] = malloc(20);
        this->isDatetime[index - 1] = true;
        SESE_DEBUG("alloc buffer addr: %p", this->buffer[index - 1]);
    } else {
        SESE_DEBUG("use buffer addr: %p", this->buffer[index - 1])
    }

    memset(this->buffer[index - 1], 0 , 20);

    std::string dateValue = text::DateTimeFormatter::format(value, "yyyy-MM-dd HH:mm:ss");
    const char *timeValue = dateValue.c_str();
    memcpy(this->buffer[index - 1], timeValue, 20);

    return SQLITE_OK == sqlite3_bind_text(stmt, (int) index, timeValue, -1, SQLITE_TRANSIENT);
}

int impl::SqlitePreparedStatementImpl::getLastError() const noexcept {
    auto conn = sqlite3_db_handle(stmt);
    if (conn) {
        return sqlite3_errcode(conn);
    } else {
        return SQLITE_MISUSE;
    }
}

const char *impl::SqlitePreparedStatementImpl::getLastErrorMessage() const noexcept {
    auto conn = sqlite3_db_handle(stmt);
    if (conn) {
        return sqlite3_errmsg(conn);
    } else {
        return nullptr;
    }
}

bool impl::SqlitePreparedStatementImpl::getColumnType(uint32_t index, MetadataType &type) noexcept {
    if (sqlite3_column_count(stmt) <= index) {
        return false;
    }

    auto raw = sqlite3_column_decltype(stmt, static_cast<int>(index));
    if (nullptr == raw) {
        return false;
    }

    // 这并不是最好的确定 SQLite 结果集元数据的最好的方法，
    // 但目前的官方提供的 API 使得我暂时只能这么做
    auto target = splitBefore(raw);
    // SESE_DEBUG("raw: %s, target: %s", raw, target.c_str());
    if (in(IntegerAffinitySet, 9, target.c_str())) {
        type = MetadataType::Integer;
    } else if (in(TextAffinitySet, 8, target.c_str())) {
        type = MetadataType::Text;
    } else if (in(RealAffinitySet, 4, target.c_str())) {
        type = MetadataType::Float;
    } else if (sese::strcmpDoNotCase("BOOLEAN", target.c_str())) {
        type = MetadataType::Boolean;
    } else if (sese::strcmpDoNotCase("DATE", target.c_str())) {
        type = MetadataType::Date;
    } else if (sese::strcmpDoNotCase("DATETIME", target.c_str())) {
        type = MetadataType::DateTime;
    } else {
        type = MetadataType::Unknown;
    }

    return true;
}

int64_t impl::SqlitePreparedStatementImpl::getColumnSize(uint32_t index) noexcept {
    if (sqlite3_column_count(stmt) <= index) {
        return -1;
    }

    std::string raw = sqlite3_column_decltype(stmt, static_cast<int>(index));
    if (raw.empty()) {
        return -1;
    }

    // 同上
    auto pos = raw.find_first_of('(');
    if (pos == std::string::npos) {
        return 0;
    } else {
        auto number = raw.substr(pos + 1, raw.length() - pos - 2);
        // SESE_DEBUG("number string: %s", number.c_str());
        char *end;
        return std::strtol(number.c_str(), &end, 10);
    }
}

