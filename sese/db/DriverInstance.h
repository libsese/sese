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

/// \file DriverInstance.h
/// \author kaoru
/// \version 0.1
/// \brief Database-driven instance class

#pragma once

#include <sese/db/Config.h>
#include <sese/db/PreparedStatement.h>
#include <sese/util/ErrorCode.h>

namespace sese::db {

    /// \brief Database-driven instance class
    class  DriverInstance {
    public:
        using Ptr = std::unique_ptr<DriverInstance>;

        /// \brief Destructors
        virtual ~DriverInstance() noexcept = default;
        /// Execute the query
        /// \param sql Query statements
        /// \return The result set that was queried
        /// \retval nullptr Query failed
        virtual ResultSet::Ptr executeQuery(const char *sql) noexcept = 0;
        /// Execute the update
        /// \param sql Update statements
        /// \return Count the number of rows updated
        /// \retval -1 Execution failed
        virtual int64_t executeUpdate(const char *sql) noexcept = 0;
        /// Creates a prepared statement object
        /// \param sql The SQL query string
        /// \return The prepared statement object
        /// \retval nullptr If creation fails
        virtual PreparedStatement::Ptr createStatement(const char *sql) noexcept = 0;

        /// Retrieves the error code corresponding to the instance's driver
        /// \return The error code
        [[nodiscard]] virtual int getLastError() const noexcept = 0;

        /// Retrieves the error message corresponding to the instance's driver (null check required)
        /// \return The error message
        [[nodiscard]] virtual const char *getLastErrorMessage() const noexcept = 0;
        /// Sets the transaction auto-commit mode
        /// \warning Since libpq does not support setting autocommit, and the autoCommit property is global, it is recommended to use `begin()` to start a transaction when using PostgreSQL.
        /// \warning Since sqlite3 does not support setting autocommit, it is recommended to use `begin()` to start a transaction when using SQLite.
        /// \param enable Enable or disable auto-commit mode
        /// \retval false If the operation fails
        [[nodiscard]] virtual bool setAutoCommit(bool enable) noexcept = 0;

        /// Checks whether auto-commit is enabled
        /// \warning Since libpq does not support retrieving the autoCommit status, it is recommended to use the command line to obtain this value when using PostgreSQL.
        /// \retval false If the operation fails
        [[nodiscard]] virtual bool getAutoCommit(bool &status) noexcept = 0;

        /// Rolls back the transaction
        /// \retval false If the rollback fails
        [[nodiscard]] virtual bool rollback() noexcept = 0;

        /// Commits the transaction
        /// \retval false If the commit fails
        [[nodiscard]] virtual bool commit() noexcept = 0;

        /// Retrieves the auto-incremented ID
        /// \warning Since libpq does not support retrieving the auto-incremented ID, it is recommended to use the command line with the query `SELECT currval(pg_get_serial_sequence('tabName', 'colName'))` to get the auto-incremented ID when using PostgreSQL.
        /// \retval false If the retrieval fails
        [[nodiscard]] virtual bool getInsertId(int64_t &id) const noexcept = 0;

        /// Starts the transaction
        /// \retval false If the transaction start fails
        [[nodiscard]] virtual bool begin() noexcept = 0;

        ErrorCode getErrorCode() const {
            return {getLastError(), getLastErrorMessage()};
        }
    };
}// namespace sese::db
