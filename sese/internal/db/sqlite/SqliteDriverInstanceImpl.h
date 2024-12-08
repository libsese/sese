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

/// \file SqliteDriverInstanceImpl.h
/// \brief SQLite driver implementation
/// \author kaoru
/// \date April 10, 2024

#pragma once

#include <sese/db/DriverInstance.h>
#include <sese/internal/db/sqlite/SqlitePreparedStatementImpl.h>

namespace sese::db::impl {

    /// \brief SQLite driver implementation
    class  SqliteDriverInstanceImpl final : public DriverInstance {
    public:
        explicit SqliteDriverInstanceImpl(sqlite3 *conn) noexcept;
        ~SqliteDriverInstanceImpl() noexcept override;
        ResultSet::Ptr executeQuery(const char *sql) noexcept override;
        int64_t executeUpdate(const char *sql) noexcept override;
        PreparedStatement::Ptr createStatement(const char *sql) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

        [[nodiscard]] bool setAutoCommit(bool enable) noexcept override;
        [[nodiscard]] bool getAutoCommit(bool &status) noexcept override;
        [[nodiscard]] bool begin() noexcept override;
        [[nodiscard]] bool commit() noexcept override;
        [[nodiscard]] bool rollback() noexcept override;

        [[nodiscard]] bool getInsertId(int64_t &id) const noexcept override;

    protected:
        sqlite3 *conn = nullptr;
    };

}// namespace sese::db::impl