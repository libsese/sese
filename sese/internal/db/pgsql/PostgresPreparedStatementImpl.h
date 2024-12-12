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

/// \file PostgresPreparedStatementImpl.h
/// \brief PSQL prepared statement implementation
/// \author kaoru, DreammerQcl
/// \date April 10, 2024

#pragma once

#include <sese/db/PreparedStatement.h>
#include <sese/internal/db/pgsql/PostgresResultSetImpl.h>
#include <random>
#include <libpq-fe.h>

namespace sese::db::impl {

    /// \brief PSQL prepared statement implementation
    class  PostgresPreparedStatementImpl : public PreparedStatement {
    public:
        explicit PostgresPreparedStatementImpl(std::string stmt_name, std::string stmt_string, uint32_t count, PGconn *conn) noexcept;
        ~PostgresPreparedStatementImpl() noexcept override;


        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        [[nodiscard]] bool setDouble(uint32_t index, const double &value) noexcept override;
        [[nodiscard]] bool setFloat(uint32_t index, const float &value) noexcept override;
        [[nodiscard]] bool setInteger(uint32_t index, const int32_t &value) noexcept override;
        [[nodiscard]] bool setLong(uint32_t index, const int64_t &value) noexcept override;
        [[nodiscard]] bool setText(uint32_t index, const char *value) noexcept override;
        [[nodiscard]] bool setNull(uint32_t index) noexcept override;
        [[nodiscard]] bool setDateTime(uint32_t index, const sese::DateTime &value) noexcept override;


        [[nodiscard]] bool getColumnType(uint32_t index, MetadataType &type) noexcept override;
        [[nodiscard]] int64_t getColumnSize(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

    protected:
        PGconn *conn;
        std::string stmtName;
        std::string stmtString;
        uint32_t count;
        Oid *paramTypes;
        const char **paramValues;
        std::string *strings;
        int error = 0;
        PGresult *result;
    };

}// namespace sese::db::impl