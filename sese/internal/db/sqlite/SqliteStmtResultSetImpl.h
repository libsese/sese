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

/// \file SqliteStmtResultSetImpl.h
/// \brief SQLite prepared statement result set
/// \author kaoru
/// \date April 10, 2024

#pragma once

#include <sese/db/ResultSet.h>
#include <sqlite3.h>

namespace sese::db::impl {

    /// \brief SQLite prepared statement result set
    class  SqliteStmtResultSetImpl final : public ResultSet {
    public:
        explicit SqliteStmtResultSetImpl(sqlite3_stmt *stmt) noexcept;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] bool isNull(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

        [[nodiscard]] int32_t getInteger(size_t index) const noexcept override;
        [[nodiscard]] int64_t getLong(size_t index) const noexcept override;
        [[nodiscard]] std::string_view getString(size_t index) const noexcept override;
        [[nodiscard]] double getDouble(size_t index) const noexcept override;
        [[nodiscard]] float getFloat(size_t index) const noexcept override;
        [[nodiscard]] std::optional<sese::DateTime> getDateTime(size_t index) const noexcept override;

    protected:
        sqlite3_stmt *stmt;
    };

}// namespace sese::db::impl