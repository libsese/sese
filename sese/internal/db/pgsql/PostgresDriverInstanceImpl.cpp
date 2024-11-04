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

#include <sese/internal/db/pgsql/PostgresDriverInstanceImpl.h>
#include <sese/internal/db/pgsql/PostgresResultSetImpl.h>
#include <sese/text/StringBuilder.h>

using namespace sese::db;

impl::PostgresDriverInstanceImpl::PostgresDriverInstanceImpl(PGconn *conn) noexcept {
    this->conn = conn;
    auto status = PQstatus(conn);
    if (status != CONNECTION_OK) {
        error = static_cast<int>(status);
    } else {
        error = 0;
    }
    result = nullptr;
}

impl::PostgresDriverInstanceImpl::~PostgresDriverInstanceImpl() noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    PQfinish(conn);
}

ResultSet::Ptr impl::PostgresDriverInstanceImpl::executeQuery(const char *sql) noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    result = PQexec(conn, sql);
    if (result == nullptr) {
        error = static_cast<int>(PQstatus(conn));
        return nullptr;
    }
    auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        error = static_cast<int>(status);
        return nullptr;
    } else {
        auto rt = std::make_unique<PostgresResultSetImpl>(result);
        error = 0;
        result = nullptr;
        return rt;
    }
}

int64_t impl::PostgresDriverInstanceImpl::executeUpdate(const char *sql) noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    result = PQexec(conn, sql);
    if (result == nullptr) {
        return -1;
    }
    auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        error = static_cast<int>(status);
        return -1;
    } else {
        auto rt = reinterpret_cast<int64_t>(PQcmdTuples(result));
        error = 0;
        PQclear(result);
        result = nullptr;
        return rt;
    }
}

PreparedStatement::Ptr impl::PostgresDriverInstanceImpl::createStatement(const char *sql) noexcept {
    text::StringBuilder string_builder;
    uint32_t count = 0;
    const std::string_view STRING{sql, strlen(sql)};
    auto begin = STRING.begin();
    while (true) {
        auto pos = std::find(begin, STRING.end(), '?');
        string_builder.append(std::string{begin, pos});
        if (pos == STRING.end()) {
            break;
        }
        string_builder << '$' << std::to_string(++count);
        begin = pos + 1;
    }

    std::string stmt_string = string_builder.toString();
    std::uniform_int_distribution<int> discrete_distribution(1, 65535);
    auto stmt_name = "sese_stmt_" + std::to_string(discrete_distribution(rd));

    auto res = PQprepare(conn, stmt_name.c_str(), stmt_string.c_str(), count, nullptr);
    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        this->error = 0;
        PQclear(res);
        return std::make_unique<impl::PostgresPreparedStatementImpl>(stmt_name, stmt_string, count, conn);
    } else {
        this->error = static_cast<int>(PQresultStatus(res));
        PQclear(res);
        return nullptr;
    }

}

int impl::PostgresDriverInstanceImpl::getLastError() const noexcept {
    return error;
}

const char *impl::PostgresDriverInstanceImpl::getLastErrorMessage() const noexcept {
    if (result) {
        return PQresultErrorMessage(result);
    } else {
        return PQerrorMessage(conn);
    }
}

bool impl::PostgresDriverInstanceImpl::setAutoCommit(bool enable) noexcept {
    return true;
}

bool impl::PostgresDriverInstanceImpl::getAutoCommit(bool &status) noexcept {
    return true;
}

bool impl::PostgresDriverInstanceImpl::commit() noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }

    result = PQexec(conn, "COMMIT;");
    if (result == nullptr) {
        return false;
    }

    auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        error = static_cast<int>(status);
        return false;
    } else {
        error = 0;
        PQclear(result);
        result = nullptr;
        return true;
    }
}


bool impl::PostgresDriverInstanceImpl::rollback() noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }

    result = PQexec(conn, "ROLLBACK;");
    if (result == nullptr) {
        return false;
    }

    auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        error = static_cast<int>(status);
        return false;
    } else {
        error = 0;
        PQclear(result);
        result = nullptr;
        return true;
    }
}


bool impl::PostgresDriverInstanceImpl::getInsertId(int64_t &id) const noexcept {
    return true;
}

bool impl::PostgresDriverInstanceImpl::begin() noexcept {
    if (result) {
        PQclear(result);
        result = nullptr;
    }

    result = PQexec(conn, "BEGIN;");

    auto status = PQresultStatus(result);

    if (status != PGRES_COMMAND_OK) {
        error = static_cast<int>(status);
        return false;
    } else {
        error = 0;
        PQclear(result);
        result = nullptr;
        return true;
    }
}
