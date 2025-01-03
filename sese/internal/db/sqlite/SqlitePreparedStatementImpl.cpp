// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/internal/db/sqlite/SqlitePreparedStatementImpl.h>

using namespace sese::db;

const char *impl::SqlitePreparedStatementImpl::INTEGER_AFFINITY_SET[9]{
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

const char *impl::SqlitePreparedStatementImpl::TEXT_AFFINITY_SET[8]{
        "CHARACTER",
        "VARCHAR",
        "VARYING CHARACTER",
        "NCHAR",
        "NATIVE CHARACTER",
        "NVARCHAR",
        "TEXT",
        "CLOB"
};

const char *impl::SqlitePreparedStatementImpl::REAL_AFFINITY_SET[6]{
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
    this->isManual = static_cast<bool *>(malloc(count));
    memset(this->isManual, false, count);
    this->buffer = static_cast<void **>(malloc(sizeof(void *) * count));
}

impl::SqlitePreparedStatementImpl::~SqlitePreparedStatementImpl() noexcept {
    for (int i = 0; i < count; ++i) {
        if (this->isManual[i]) {
            free(buffer[i]);
        }
    }
    free(isManual);
    free(buffer);
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
        return sqlite3_changes(conn);
    } else {
        return -1;
    }
}

bool impl::SqlitePreparedStatementImpl::setDouble(uint32_t index, const double &value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
        this->isManual[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, static_cast<int>(index), value);
}

bool impl::SqlitePreparedStatementImpl::setFloat(uint32_t index, const float &value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
        this->isManual[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_double(stmt, static_cast<int>(index), value);
}

bool impl::SqlitePreparedStatementImpl::setLong(uint32_t index, const int64_t &value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
        this->isManual[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int64(stmt, static_cast<int>(index), value);
}

bool impl::SqlitePreparedStatementImpl::setInteger(uint32_t index, const int32_t &value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
        this->isManual[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_int(stmt, static_cast<int>(index), value);
}

bool impl::SqlitePreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
    }

    this->isManual[index - 1] = true;
    auto len = strlen(value);
    this->buffer[index - 1] = malloc(len + 1);
    static_cast<char *>(this->buffer[index - 1])[len] = 0;
    memcpy(this->buffer[index - 1], value, len);

    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_text(stmt, static_cast<int>(index), value, -1, nullptr);
}

bool impl::SqlitePreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
        this->isManual[index - 1] = false;
    }
    this->stmtStatus = false;
    return SQLITE_OK == sqlite3_bind_null(stmt, static_cast<int>(index));
}

bool impl::SqlitePreparedStatementImpl::setDateTime(uint32_t index, const sese::DateTime &value) noexcept {
    if (index == 0 || index > count) return false;
    if (this->stmtStatus) sqlite3_reset(stmt);
    this->stmtStatus = false;

    if (this->isManual[index - 1]) {
        free(this->buffer[index - 1]);
    }

    this->buffer[index - 1] = malloc(20);
    this->isManual[index - 1] = true;

    std::string date_value = text::DateTimeFormatter::format(value, "yyyy-MM-dd HH:mm:ss");
    memcpy(this->buffer[index - 1], date_value.c_str(), 20);

    return SQLITE_OK == sqlite3_bind_text(stmt, static_cast<int>(index), static_cast<const char *>(this->buffer[index - 1]), -1, SQLITE_TRANSIENT);
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
    if (sqlite3_column_count(stmt) <= static_cast<int>(index)) {
        return false;
    }

    auto raw = sqlite3_column_decltype(stmt, static_cast<int>(index));
    if (nullptr == raw) {
        return false;
    }

    // This is not the best way to determine SQLite result set metadata,
    // but the currently available official API leaves me with no better option for now
    auto target = splitBefore(raw);
    // SESE_DEBUG("raw: %s, target: %s", raw, target.c_str());
    if (in(INTEGER_AFFINITY_SET, 9, target.c_str())) {
        type = MetadataType::INTEGER;
    } else if (in(TEXT_AFFINITY_SET, 8, target.c_str())) {
        type = MetadataType::TEXT;
    } else if (in(REAL_AFFINITY_SET, 4, target.c_str())) {
        type = MetadataType::FLOAT;
    } else if (sese::strcmpDoNotCase("BOOLEAN", target.c_str())) {
        type = MetadataType::BOOLEAN;
    } else if (sese::strcmpDoNotCase("DATE", target.c_str())) {
        type = MetadataType::DATE;
    } else if (sese::strcmpDoNotCase("DATETIME", target.c_str())) {
        type = MetadataType::DATE_TIME;
    } else {
        type = MetadataType::UNKNOWN;
    }

    return true;
}

int64_t impl::SqlitePreparedStatementImpl::getColumnSize(uint32_t index) noexcept {
    if (sqlite3_column_count(stmt) <= static_cast<int>(index)) {
        return -1;
    }

    std::string raw = sqlite3_column_decltype(stmt, static_cast<int>(index));
    if (raw.empty()) {
        return -1;
    }

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

