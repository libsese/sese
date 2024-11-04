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

#include <sese/internal/db/maria/MariaResultSetImpl.h>
#include <sese/text/DateTimeParser.h>

using namespace sese::db;

impl::MariaResultSetImpl::MariaResultSetImpl(MYSQL_RES *res) noexcept : ResultSet() {
    this->res = res;
}

impl::MariaResultSetImpl::~MariaResultSetImpl() noexcept {
    mysql_free_result(res);
}

void impl::MariaResultSetImpl::reset() noexcept {
    mysql_data_seek(res, 0);
}

bool impl::MariaResultSetImpl::next() noexcept {
    row = mysql_fetch_row(res);
    return row != nullptr;
}

//const char *impl::MariaResultSetImpl::getColumnByIndex(size_t index) const noexcept {
//    return row[index];
//}

size_t impl::MariaResultSetImpl::getColumns() const noexcept {
    return mysql_num_fields(res);
}

int32_t impl::MariaResultSetImpl::getInteger(size_t index) const noexcept {
    char *end;
    return static_cast<int32_t>(std::strtol(row[index], &end, 10));
}

int64_t impl::MariaResultSetImpl::getLong(size_t index) const noexcept {
    char *end;
    return std::strtoll(row[index], &end, 10);
}

std::string_view impl::MariaResultSetImpl::getString(size_t index) const noexcept {
    return {row[index]};
}

double impl::MariaResultSetImpl::getDouble(size_t index) const noexcept {
    char *end;
    return std::strtod(row[index], &end);
}
float impl::MariaResultSetImpl::getFloat(size_t index) const noexcept {
    char *end;
    return std::strtof(row[index], &end);
}
std::optional<sese::DateTime> impl::MariaResultSetImpl::getDateTime(size_t index) const noexcept {
    std::optional<sese::DateTime> rt = text::DateTimeParser::parse("yyyy-MM-dd HH:mm:ss", row[index]);
    return rt;
}
bool impl::MariaResultSetImpl::isNull(size_t index) const noexcept {
    return row[index] == nullptr;
}
