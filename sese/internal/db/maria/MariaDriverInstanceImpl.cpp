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

#include <sese/internal/db/maria/MariaDriverInstanceImpl.h>
#include <sese/internal/db/maria/MariaResultSetImpl.h>

using namespace sese::db;

impl::MariaDriverInstanceImpl::MariaDriverInstanceImpl(MYSQL *conn) noexcept {
    this->conn = conn;
}

impl::MariaDriverInstanceImpl::~MariaDriverInstanceImpl() noexcept {
    mysql_close(conn);
}

ResultSet::Ptr impl::MariaDriverInstanceImpl::executeQuery(const char *sql) noexcept {
    if (0 != mysql_query(conn, sql)) {
        return nullptr;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    return std::make_unique<MariaResultSetImpl>(result);
}

int64_t impl::MariaDriverInstanceImpl::executeUpdate(const char *sql) noexcept {
    if (0 != mysql_query(conn, sql)) {
        return -1;
    }
    return static_cast<long long>(mysql_affected_rows(conn));
}

PreparedStatement::Ptr impl::MariaDriverInstanceImpl::createStatement(const char *sql) noexcept {
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) return nullptr;

    size_t len = strlen(sql);
    if (mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(len))) return nullptr;

    // 此处需要手动计算 '?' 个数
    size_t count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (sql[i] == '?') count++;
    }

    // 获取结果集元数据，对于非 DQL 而言这可能是 nullptr，不应作为错误处理
    auto meta = mysql_stmt_result_metadata(stmt);

    return std::make_unique<impl::MariaPreparedStatementImpl>(stmt, meta, count);
}

int impl::MariaDriverInstanceImpl::getLastError() const noexcept {
    return static_cast<int>(mysql_errno(conn));
}

const char *impl::MariaDriverInstanceImpl::getLastErrorMessage() const noexcept {
    return mysql_error(conn);
}

bool impl::MariaDriverInstanceImpl::setAutoCommit(bool enable) noexcept {
    int temp;
    if (enable) {
        temp = static_cast<unsigned char>(mysql_autocommit(conn, 1));
    } else {
        temp = static_cast<unsigned char>(mysql_autocommit(conn, 0));
    }
    return temp == 0;
}

bool impl::MariaDriverInstanceImpl::getAutoCommit(bool &status) noexcept {
    auto rt = executeQuery("show variables like 'autocommit';");
    if (rt) {
        while (rt->next()) {
            status = rt->getString(1) == "ON";
        }
        return true;
    } else {
        return false;
    }
}

bool impl::MariaDriverInstanceImpl::commit() noexcept {
    int comm = static_cast<unsigned char>(mysql_commit(conn));
    return comm == 0;
}

bool impl::MariaDriverInstanceImpl::rollback() noexcept {
    int back = static_cast<unsigned char>(mysql_rollback(conn));
    return back == 0;
}

bool impl::MariaDriverInstanceImpl::getInsertId(int64_t &id) const noexcept {
    id = static_cast<int64_t>(mysql_insert_id(conn));
    if (id) {
        return true;
    } else {
        return false;
    }
}

bool impl::MariaDriverInstanceImpl::begin() noexcept {
    return mysql_query(conn, "BEGIN;") == 0;
}