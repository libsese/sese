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

#include <sese/internal/db/maria/MariaStmtResultSetImpl.h>
#include <sese/internal/db/maria/MariaPreparedStatementImpl.h>
#include <ctime>

sese::db::impl::MariaStmtResultSet::MariaStmtResultSet(MYSQL_STMT *stmt, MYSQL_BIND *row, size_t count) noexcept {
    this->stmt = stmt;
    this->row = row;
    this->columns = count;
}

sese::db::impl::MariaStmtResultSet::~MariaStmtResultSet() noexcept {
    MariaPreparedStatementImpl::freeBindStruct(this->row, this->columns);
}

void sese::db::impl::MariaStmtResultSet::reset() noexcept {
    mysql_stmt_data_seek(stmt, 0);
}

bool sese::db::impl::MariaStmtResultSet::next() noexcept {
    return 0 == mysql_stmt_fetch(stmt);
}

size_t sese::db::impl::MariaStmtResultSet::getColumns() const noexcept {
    return columns;
}

int64_t sese::db::impl::MariaStmtResultSet::getLong(size_t index) const noexcept {
    int64_t value;
    value = *static_cast<int64_t *>(row[index].buffer);
    return value;
}

int32_t sese::db::impl::MariaStmtResultSet::getInteger(size_t index) const noexcept {
    int32_t value;
    value = *static_cast<int32_t *>(row[index].buffer);
    return value;
}

std::string_view sese::db::impl::MariaStmtResultSet::getString(size_t index) const noexcept {
    return {static_cast<const char *>(row[index].buffer)};
}

double sese::db::impl::MariaStmtResultSet::getDouble(size_t index) const noexcept {
    double value;
    value = *static_cast<double *>(row[index].buffer);
    return value;
}

float sese::db::impl::MariaStmtResultSet::getFloat(size_t index) const noexcept {
    float value;
    value = *static_cast<float *>(row[index].buffer);
    return value;
}

std::optional<sese::DateTime> sese::db::impl::MariaStmtResultSet::getDateTime(size_t index) const noexcept {
    // 未来需要详细的 DateTime 构造函数
    auto datetime = static_cast<MYSQL_TIME *>(row[index].buffer);
    std::tm tm{};
    tm.tm_year = datetime->year - 1900;
    tm.tm_mon = datetime->month - 1;
    tm.tm_mday = datetime->day;
    tm.tm_hour = datetime->hour;
    tm.tm_min = datetime->minute;
    tm.tm_sec = datetime->second;
    auto timestamp = timegm(&tm);
    if (-1 == timestamp) {
        return std::nullopt;
    }
    return sese::DateTime(timestamp * 1000000, 0, sese::DateTime::Policy::FORMAT);
}

bool sese::db::impl::MariaStmtResultSet::isNull(size_t index) const noexcept {
    return *row[index].is_null == 1;
}
